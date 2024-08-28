# SPDX-License-Identifier: GPL-2.0+
# Copyright (c) 2022-2023 Texas Instruments Incorporated - https://www.ti.com/
# Written by Neha Malcom Francis <n-francis@ti.com>
#

# Support for generation of TI secured binary blobs

from binman.entry import EntryArg
from binman.etype.x509_cert import Entry_x509_cert
from dataclasses import dataclass

from dtoc import fdt_util

@dataclass
class Firewall():
    id: int
    region: int
    control : int
    permissions: list
    start_address: str
    end_address: str

    def ensure_props(self, etype, name):
        missing_props = []
        for key, val in self.__dict__.items():
            if val is None:
                missing_props += [key]

        if len(missing_props):
            etype.Raise(f"Subnode '{name}' is missing properties: {','.join(missing_props)}")

    def get_certificate(self) -> str:
        unique_identifier = f"{self.id}{self.region}"
        cert = f"""
firewallID{unique_identifier} = INTEGER:{self.id}
region{unique_identifier} = INTEGER:{self.region}
control{unique_identifier} = INTEGER:{hex(self.control)}
nPermissionRegs{unique_identifier} = INTEGER:{len(self.permissions)}
"""
        for index, permission in enumerate(self.permissions):
            cert += f"""permissions{unique_identifier}{index} = INTEGER:{hex(permission)}
"""
        cert += f"""startAddress{unique_identifier} = FORMAT:HEX,OCT:{self.start_address:02x}
endAddress{unique_identifier} = FORMAT:HEX,OCT:{self.end_address:02x}
"""
        return cert

class Entry_ti_secure(Entry_x509_cert):
    """Entry containing a TI x509 certificate binary

    Properties / Entry arguments:
        - content: List of phandles to entries to sign
        - keyfile: Filename of file containing key to sign binary with
        - sha: Hash function to be used for signing
        - auth-in-place: This is an integer field that contains two pieces
          of information:

            - Lower Byte - Remains 0x02 as per our use case
              ( 0x02: Move the authenticated binary back to the header )
            - Upper Byte - The Host ID of the core owning the firewall

    Output files:
        - input.<unique_name> - input file passed to openssl
        - config.<unique_name> - input file generated for openssl (which is
          used as the config file)
        - cert.<unique_name> - output file generated by openssl (which is
          used as the entry contents)

    Depending on auth-in-place information in the inputs, we read the
    firewall nodes that describe the configurations of firewall that TIFS
    will be doing after reading the certificate.

    The syntax of the firewall nodes are as such::

        firewall-257-0 {
            id = <257>;           /* The ID of the firewall being configured */
            region = <0>;         /* Region number to configure */

            control =             /* The control register */
                <(FWCTRL_EN | FWCTRL_LOCK | FWCTRL_BG | FWCTRL_CACHE)>;

            permissions =         /* The permission registers */
                <((FWPRIVID_ALL << FWPRIVID_SHIFT) |
                            FWPERM_SECURE_PRIV_RWCD |
                            FWPERM_SECURE_USER_RWCD |
                            FWPERM_NON_SECURE_PRIV_RWCD |
                            FWPERM_NON_SECURE_USER_RWCD)>;

            /* More defines can be found in k3-security.h */

            start_address =        /* The Start Address of the firewall */
                <0x0 0x0>;
            end_address =          /* The End Address of the firewall */
                <0xff 0xffffffff>;
        };


    openssl signs the provided data, using the TI templated config file and
    writes the signature in this entry. This allows verification that the
    data is genuine.
    """
    def __init__(self, section, etype, node):
        super().__init__(section, etype, node)
        self.openssl = None
        self.firewall_cert_data: dict = {
            'auth_in_place': 0x02,
            'num_firewalls': 0,
            'certificate': '',
        }

    def ReadNode(self):
        super().ReadNode()
        self.key_fname = self.GetEntryArgsOrProps([
            EntryArg('keyfile', str)], required=True)[0]
        auth_in_place = fdt_util.GetInt(self._node, 'auth-in-place')
        if auth_in_place:
            self.firewall_cert_data['auth_in_place'] = auth_in_place
            self.ReadFirewallNode()
        self.sha = fdt_util.GetInt(self._node, 'sha', 512)
        self.req_dist_name = {'C': 'US',
                'ST': 'TX',
                'L': 'Dallas',
                'O': 'Texas Instruments Incorporated',
                'OU': 'Processors',
                'CN': 'TI Support',
                'emailAddress': 'support@ti.com'}

    def ReadFirewallNode(self):
        self.firewall_cert_data['certificate'] = ""
        self.firewall_cert_data['num_firewalls'] = 0
        for node in self._node.subnodes:
            if 'firewall' in node.name:
                firewall = Firewall(
                     fdt_util.GetInt(node, 'id'),
                     fdt_util.GetInt(node, 'region'),
                     fdt_util.GetInt(node, 'control'),
                     fdt_util.GetPhandleList(node, 'permissions'),
                     fdt_util.GetInt64(node, 'start_address'),
                     fdt_util.GetInt64(node, 'end_address'),
                )
                firewall.ensure_props(self, node.name)
                self.firewall_cert_data['num_firewalls'] += 1
                self.firewall_cert_data['certificate'] += firewall.get_certificate()

    def GetCertificate(self, required):
        """Get the contents of this entry

        Args:
            required: True if the data must be present, False if it is OK to
                return None

        Returns:
            bytes content of the entry, which is the certificate binary for the
                provided data
        """
        return super().GetCertificate(required=required, type='sysfw')

    def ObtainContents(self):
        data = self.data
        if data is None:
            data = self.GetCertificate(False)
        if data is None:
            return False
        self.SetContents(data)
        return True

    def ProcessContents(self):
        # The blob may have changed due to WriteSymbols()
        data = self.data
        return self.ProcessContentsUpdate(data)

    def AddBintools(self, btools):
        super().AddBintools(btools)
        self.openssl = self.AddBintool(btools, 'openssl')
