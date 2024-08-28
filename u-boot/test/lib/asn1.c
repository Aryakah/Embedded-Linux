// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2019 Linaro Limited
 * Author: AKASHI Takahiro
 *
 * Unit test for asn1 compiler and asn1 decoder function via various parsers
 */

#include <command.h>
#include <test/lib.h>
#include <test/test.h>
#include <test/ut.h>

#ifdef CONFIG_PKCS7_MESSAGE_PARSER
#include <crypto/pkcs7_parser.h>
#else
#ifdef CONFIG_X509_CERTIFICATE_PARSER
#include <crypto/x509_parser.h>
#endif
#endif

#ifdef CONFIG_X509_CERTIFICATE_PARSER
static const unsigned char cert_data[] = {
	0x30, 0x82, 0x03, 0xc7, 0x30, 0x82, 0x02, 0xaf, 0xa0, 0x03, 0x02, 0x01,
	0x02, 0x02, 0x09, 0x00, 0xd7, 0x17, 0x0a, 0x76, 0xd5, 0xd3, 0x4d, 0xeb,
	0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
	0x0b, 0x05, 0x00, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
	0x04, 0x06, 0x13, 0x02, 0x4a, 0x50, 0x31, 0x0e, 0x30, 0x0c, 0x06, 0x03,
	0x55, 0x04, 0x08, 0x0c, 0x05, 0x54, 0x6f, 0x6b, 0x79, 0x6f, 0x31, 0x0e,
	0x30, 0x0c, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x05, 0x54, 0x6f, 0x6b,
	0x79, 0x6f, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c,
	0x06, 0x4c, 0x69, 0x6e, 0x61, 0x72, 0x6f, 0x31, 0x0b, 0x30, 0x09, 0x06,
	0x03, 0x55, 0x04, 0x0b, 0x0c, 0x02, 0x53, 0x57, 0x31, 0x0f, 0x30, 0x0d,
	0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x06, 0x54, 0x65, 0x73, 0x74, 0x65,
	0x72, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x09, 0x01, 0x16, 0x0d, 0x74, 0x65, 0x73, 0x74, 0x40, 0x74,
	0x65, 0x73, 0x74, 0x2e, 0x6f, 0x72, 0x67, 0x30, 0x1e, 0x17, 0x0d, 0x31,
	0x39, 0x31, 0x30, 0x31, 0x38, 0x30, 0x33, 0x31, 0x33, 0x33, 0x31, 0x5a,
	0x17, 0x0d, 0x32, 0x30, 0x31, 0x30, 0x31, 0x37, 0x30, 0x33, 0x31, 0x33,
	0x33, 0x31, 0x5a, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
	0x04, 0x06, 0x13, 0x02, 0x4a, 0x50, 0x31, 0x0e, 0x30, 0x0c, 0x06, 0x03,
	0x55, 0x04, 0x08, 0x0c, 0x05, 0x54, 0x6f, 0x6b, 0x79, 0x6f, 0x31, 0x0e,
	0x30, 0x0c, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x05, 0x54, 0x6f, 0x6b,
	0x79, 0x6f, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c,
	0x06, 0x4c, 0x69, 0x6e, 0x61, 0x72, 0x6f, 0x31, 0x0b, 0x30, 0x09, 0x06,
	0x03, 0x55, 0x04, 0x0b, 0x0c, 0x02, 0x53, 0x57, 0x31, 0x0f, 0x30, 0x0d,
	0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x06, 0x54, 0x65, 0x73, 0x74, 0x65,
	0x72, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x09, 0x01, 0x16, 0x0d, 0x74, 0x65, 0x73, 0x74, 0x40, 0x74,
	0x65, 0x73, 0x74, 0x2e, 0x6f, 0x72, 0x67, 0x30, 0x82, 0x01, 0x22, 0x30,
	0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01,
	0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02,
	0x82, 0x01, 0x01, 0x00, 0x9f, 0x37, 0x4d, 0x95, 0x7e, 0x36, 0xb7, 0xaf,
	0xf4, 0xd6, 0xce, 0x39, 0x04, 0xee, 0xbf, 0x36, 0xb2, 0xcc, 0xa3, 0x8b,
	0x9e, 0xac, 0x62, 0x8a, 0xe9, 0xae, 0x18, 0xcf, 0xe8, 0x95, 0xfd, 0xcb,
	0xad, 0x34, 0x8a, 0x5f, 0x55, 0xe6, 0x0c, 0x5e, 0xf8, 0x76, 0xc1, 0xa2,
	0xc3, 0xd4, 0x73, 0x13, 0x8a, 0x71, 0x1b, 0xfd, 0x58, 0x27, 0xea, 0x4d,
	0x41, 0xff, 0x63, 0xbb, 0xad, 0x97, 0x62, 0xba, 0xe4, 0xe5, 0x97, 0x45,
	0xa3, 0x5b, 0xd5, 0x5b, 0x53, 0x55, 0x10, 0x19, 0xfa, 0xac, 0xbd, 0xdb,
	0x77, 0x62, 0x23, 0x50, 0x3f, 0x35, 0xdb, 0x8a, 0xf6, 0xee, 0x7a, 0x31,
	0xec, 0x92, 0xf5, 0x78, 0x35, 0x92, 0x76, 0x3c, 0x5f, 0xe7, 0xee, 0xc9,
	0xed, 0x01, 0x1c, 0x42, 0x55, 0xd6, 0x7e, 0xa6, 0xca, 0x7c, 0xd1, 0x15,
	0x16, 0x87, 0x7c, 0x99, 0x63, 0xc0, 0xa9, 0x25, 0x49, 0xbc, 0x4e, 0xdc,
	0x2d, 0x4b, 0xcb, 0x52, 0xd7, 0x67, 0xe9, 0x83, 0x6b, 0x5e, 0x5b, 0x48,
	0x80, 0x33, 0xe9, 0xcc, 0xe8, 0xfe, 0x19, 0xc8, 0xc2, 0x61, 0x74, 0x52,
	0x25, 0x92, 0x48, 0xea, 0xad, 0x15, 0x16, 0x64, 0x6e, 0x53, 0x30, 0x77,
	0xa2, 0xef, 0x61, 0x92, 0x1b, 0x5e, 0xbe, 0x07, 0xf2, 0x3c, 0xf8, 0x35,
	0x7d, 0x76, 0x4f, 0x78, 0xa9, 0x2a, 0xf1, 0x32, 0xff, 0xec, 0x89, 0xa9,
	0x22, 0x4c, 0x3d, 0xc8, 0x65, 0xca, 0xf4, 0xa2, 0x6d, 0x3f, 0xa4, 0x0a,
	0xfa, 0x9e, 0xe4, 0xf0, 0xdb, 0x39, 0xb1, 0xf9, 0xf0, 0xfb, 0x04, 0x81,
	0x44, 0xa7, 0xd7, 0x61, 0xdf, 0x2d, 0x13, 0x45, 0x2c, 0xae, 0xf0, 0x0e,
	0xc4, 0x07, 0x5d, 0x7d, 0x2b, 0xb2, 0x20, 0x75, 0x33, 0x6b, 0x5b, 0xf7,
	0xe7, 0x17, 0x51, 0xf1, 0xab, 0xc1, 0x9e, 0xc6, 0xf0, 0x30, 0xc6, 0x25,
	0x26, 0x3e, 0xd7, 0xd7, 0xa3, 0xcc, 0x15, 0x95, 0x02, 0x03, 0x01, 0x00,
	0x01, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
	0x04, 0x16, 0x04, 0x14, 0x45, 0x8a, 0x76, 0xf7, 0x4f, 0xf4, 0x0e, 0xa0,
	0xf2, 0x02, 0xe1, 0xe7, 0xe9, 0xc7, 0x7d, 0x51, 0x55, 0x92, 0x33, 0xcd,
	0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80,
	0x14, 0x45, 0x8a, 0x76, 0xf7, 0x4f, 0xf4, 0x0e, 0xa0, 0xf2, 0x02, 0xe1,
	0xe7, 0xe9, 0xc7, 0x7d, 0x51, 0x55, 0x92, 0x33, 0xcd, 0x30, 0x0c, 0x06,
	0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30,
	0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
	0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x47, 0x93, 0x82, 0x0e, 0x8a,
	0x70, 0x9d, 0x6c, 0x7a, 0xdb, 0x04, 0xb4, 0xc9, 0xef, 0x98, 0x28, 0xc6,
	0xd9, 0x53, 0x90, 0xc8, 0x25, 0x83, 0x07, 0x23, 0xe7, 0x59, 0x38, 0xc1,
	0xc0, 0x50, 0x28, 0x99, 0x92, 0xfb, 0x21, 0x24, 0x72, 0xe5, 0xa6, 0x57,
	0x30, 0x31, 0xb3, 0xdf, 0xa0, 0x17, 0xa9, 0x73, 0x9c, 0x39, 0x83, 0xfb,
	0xe4, 0xfa, 0x20, 0x1d, 0xfa, 0x33, 0x20, 0x0c, 0x72, 0x2a, 0x50, 0x40,
	0xbd, 0x2d, 0x33, 0xa2, 0xfc, 0x06, 0xf9, 0xfe, 0x86, 0x4f, 0x50, 0x1d,
	0x65, 0x37, 0xe9, 0x30, 0x33, 0x82, 0xa1, 0x75, 0x8f, 0x5d, 0x33, 0x84,
	0x0d, 0xf2, 0x09, 0x04, 0xc0, 0x7a, 0x12, 0x79, 0xdb, 0x4f, 0x77, 0x04,
	0xe4, 0xd8, 0x0b, 0x87, 0x19, 0xba, 0xb7, 0x3c, 0xa6, 0x45, 0xaa, 0x91,
	0x62, 0x7f, 0x01, 0x7d, 0xc6, 0x20, 0x6d, 0x71, 0x15, 0x74, 0x5e, 0x87,
	0xb3, 0x60, 0x17, 0x9c, 0xc0, 0xed, 0x01, 0x4b, 0xb3, 0x23, 0x24, 0xc1,
	0xcb, 0x7a, 0x83, 0x03, 0x26, 0x2d, 0xde, 0x47, 0xc5, 0x11, 0x94, 0x28,
	0x27, 0x15, 0x92, 0x00, 0x8b, 0x2e, 0x51, 0x42, 0xca, 0x4b, 0x4a, 0x2c,
	0x51, 0x37, 0x56, 0xd0, 0xbc, 0x33, 0xd5, 0xd5, 0x3e, 0x79, 0x5c, 0x3f,
	0x9d, 0x6e, 0xb1, 0xe9, 0x71, 0xf1, 0x2c, 0xe9, 0xb4, 0x88, 0x2c, 0xd2,
	0x49, 0x97, 0xce, 0x29, 0x94, 0x16, 0xc9, 0xf9, 0x64, 0x0e, 0xd0, 0xd9,
	0x7a, 0x53, 0x10, 0x1a, 0xee, 0x83, 0x73, 0x93, 0x1b, 0xdf, 0x8a, 0x77,
	0xc0, 0x56, 0x63, 0xab, 0x5a, 0x65, 0xc5, 0xc5, 0x3b, 0xf3, 0x30, 0x80,
	0xfc, 0x38, 0x8b, 0xc9, 0xcd, 0xc3, 0x4f, 0x2e, 0x2d, 0x67, 0xcc, 0x17,
	0x18, 0x9b, 0x3e, 0xc6, 0x47, 0x03, 0xfc, 0x35, 0xa8, 0x35, 0x06, 0x5a,
	0x77, 0xe5, 0x97, 0x71, 0xbb, 0x27, 0x93, 0x0d, 0x1f, 0x0e, 0x8c
};

static unsigned int cert_data_len = 971;

/**
 * lib_asn1_x509() - unit test for asn1 decoder function
 * with x509 certificate parser
 *
 * @uts:	unit test state
 * Return:	0 = success, 1 = failure
 */
static int lib_asn1_x509(struct unit_test_state *uts)
{
	struct x509_certificate *cert;

	cert = x509_cert_parse(cert_data, cert_data_len);

	ut_assertf(!IS_ERR(cert), "decoding failed\n");
	ut_assertf(!strcmp(cert->subject, "Linaro: Tester"),
		   "subject doesn't match\n");
	ut_assertf(!strcmp(cert->issuer, "Linaro: Tester"),
		   "issuer doesn't match\n");
	ut_assertf(cert->pub, "public key doesn't exist\n");
	ut_assertf(cert->pub->keylen == 0x10e, "key length doesn't match\n");
	ut_assertf(!strcmp(cert->pub->pkey_algo, "rsa"), "algo isn't rsa\n");
	ut_assertf(cert->valid_from == 0x5da92ddb,
		   "valid_from doesn't match\n");
	ut_assertf(cert->valid_to == 0x5f8a615b, "valid_to doesn't match\n");

	x509_free_certificate(cert);

	return CMD_RET_SUCCESS;
}

LIB_TEST(lib_asn1_x509, 0);
#endif /* CONFIG_X509_CERTIFICATE_PARSER */

#ifdef CONFIG_PKCS7_MESSAGE_PARSER
/*
 * sbsign --key priv.pem --cert cert.pem --detach --out Image.pk Image
 */
static const unsigned char image_pk7[] = {
	0x30, 0x82, 0x07, 0x0f, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
	0x01, 0x07, 0x02, 0xa0, 0x82, 0x07, 0x00, 0x30, 0x82, 0x06, 0xfc, 0x02,
	0x01, 0x01, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
	0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x30, 0x78, 0x06, 0x0a, 0x2b,
	0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02, 0x01, 0x04, 0xa0, 0x6a, 0x30,
	0x68, 0x30, 0x33, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37,
	0x02, 0x01, 0x0f, 0x30, 0x25, 0x03, 0x01, 0x00, 0xa0, 0x20, 0xa2, 0x1e,
	0x80, 0x1c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x4f, 0x00, 0x62,
	0x00, 0x73, 0x00, 0x6f, 0x00, 0x6c, 0x00, 0x65, 0x00, 0x74, 0x00, 0x65,
	0x00, 0x3e, 0x00, 0x3e, 0x00, 0x3e, 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09,
	0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04,
	0x20, 0x9e, 0x90, 0x99, 0x6d, 0xf2, 0xb5, 0x3d, 0x3f, 0xfc, 0x38, 0xb6,
	0xf2, 0x1f, 0xd2, 0x24, 0x88, 0x43, 0x77, 0x7d, 0xc1, 0x2c, 0x9e, 0x8a,
	0xf6, 0xf7, 0xdd, 0x9e, 0x9c, 0x5f, 0x18, 0x36, 0xc5, 0xa0, 0x82, 0x03,
	0xcb, 0x30, 0x82, 0x03, 0xc7, 0x30, 0x82, 0x02, 0xaf, 0xa0, 0x03, 0x02,
	0x01, 0x02, 0x02, 0x09, 0x00, 0xd7, 0x17, 0x0a, 0x76, 0xd5, 0xd3, 0x4d,
	0xeb, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
	0x01, 0x0b, 0x05, 0x00, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
	0x55, 0x04, 0x06, 0x13, 0x02, 0x4a, 0x50, 0x31, 0x0e, 0x30, 0x0c, 0x06,
	0x03, 0x55, 0x04, 0x08, 0x0c, 0x05, 0x54, 0x6f, 0x6b, 0x79, 0x6f, 0x31,
	0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x05, 0x54, 0x6f,
	0x6b, 0x79, 0x6f, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a,
	0x0c, 0x06, 0x4c, 0x69, 0x6e, 0x61, 0x72, 0x6f, 0x31, 0x0b, 0x30, 0x09,
	0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x02, 0x53, 0x57, 0x31, 0x0f, 0x30,
	0x0d, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x06, 0x54, 0x65, 0x73, 0x74,
	0x65, 0x72, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
	0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 0x0d, 0x74, 0x65, 0x73, 0x74, 0x40,
	0x74, 0x65, 0x73, 0x74, 0x2e, 0x6f, 0x72, 0x67, 0x30, 0x1e, 0x17, 0x0d,
	0x31, 0x39, 0x31, 0x30, 0x31, 0x38, 0x30, 0x33, 0x31, 0x33, 0x33, 0x31,
	0x5a, 0x17, 0x0d, 0x32, 0x30, 0x31, 0x30, 0x31, 0x37, 0x30, 0x33, 0x31,
	0x33, 0x33, 0x31, 0x5a, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
	0x55, 0x04, 0x06, 0x13, 0x02, 0x4a, 0x50, 0x31, 0x0e, 0x30, 0x0c, 0x06,
	0x03, 0x55, 0x04, 0x08, 0x0c, 0x05, 0x54, 0x6f, 0x6b, 0x79, 0x6f, 0x31,
	0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x05, 0x54, 0x6f,
	0x6b, 0x79, 0x6f, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a,
	0x0c, 0x06, 0x4c, 0x69, 0x6e, 0x61, 0x72, 0x6f, 0x31, 0x0b, 0x30, 0x09,
	0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x02, 0x53, 0x57, 0x31, 0x0f, 0x30,
	0x0d, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x06, 0x54, 0x65, 0x73, 0x74,
	0x65, 0x72, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
	0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 0x0d, 0x74, 0x65, 0x73, 0x74, 0x40,
	0x74, 0x65, 0x73, 0x74, 0x2e, 0x6f, 0x72, 0x67, 0x30, 0x82, 0x01, 0x22,
	0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
	0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a,
	0x02, 0x82, 0x01, 0x01, 0x00, 0x9f, 0x37, 0x4d, 0x95, 0x7e, 0x36, 0xb7,
	0xaf, 0xf4, 0xd6, 0xce, 0x39, 0x04, 0xee, 0xbf, 0x36, 0xb2, 0xcc, 0xa3,
	0x8b, 0x9e, 0xac, 0x62, 0x8a, 0xe9, 0xae, 0x18, 0xcf, 0xe8, 0x95, 0xfd,
	0xcb, 0xad, 0x34, 0x8a, 0x5f, 0x55, 0xe6, 0x0c, 0x5e, 0xf8, 0x76, 0xc1,
	0xa2, 0xc3, 0xd4, 0x73, 0x13, 0x8a, 0x71, 0x1b, 0xfd, 0x58, 0x27, 0xea,
	0x4d, 0x41, 0xff, 0x63, 0xbb, 0xad, 0x97, 0x62, 0xba, 0xe4, 0xe5, 0x97,
	0x45, 0xa3, 0x5b, 0xd5, 0x5b, 0x53, 0x55, 0x10, 0x19, 0xfa, 0xac, 0xbd,
	0xdb, 0x77, 0x62, 0x23, 0x50, 0x3f, 0x35, 0xdb, 0x8a, 0xf6, 0xee, 0x7a,
	0x31, 0xec, 0x92, 0xf5, 0x78, 0x35, 0x92, 0x76, 0x3c, 0x5f, 0xe7, 0xee,
	0xc9, 0xed, 0x01, 0x1c, 0x42, 0x55, 0xd6, 0x7e, 0xa6, 0xca, 0x7c, 0xd1,
	0x15, 0x16, 0x87, 0x7c, 0x99, 0x63, 0xc0, 0xa9, 0x25, 0x49, 0xbc, 0x4e,
	0xdc, 0x2d, 0x4b, 0xcb, 0x52, 0xd7, 0x67, 0xe9, 0x83, 0x6b, 0x5e, 0x5b,
	0x48, 0x80, 0x33, 0xe9, 0xcc, 0xe8, 0xfe, 0x19, 0xc8, 0xc2, 0x61, 0x74,
	0x52, 0x25, 0x92, 0x48, 0xea, 0xad, 0x15, 0x16, 0x64, 0x6e, 0x53, 0x30,
	0x77, 0xa2, 0xef, 0x61, 0x92, 0x1b, 0x5e, 0xbe, 0x07, 0xf2, 0x3c, 0xf8,
	0x35, 0x7d, 0x76, 0x4f, 0x78, 0xa9, 0x2a, 0xf1, 0x32, 0xff, 0xec, 0x89,
	0xa9, 0x22, 0x4c, 0x3d, 0xc8, 0x65, 0xca, 0xf4, 0xa2, 0x6d, 0x3f, 0xa4,
	0x0a, 0xfa, 0x9e, 0xe4, 0xf0, 0xdb, 0x39, 0xb1, 0xf9, 0xf0, 0xfb, 0x04,
	0x81, 0x44, 0xa7, 0xd7, 0x61, 0xdf, 0x2d, 0x13, 0x45, 0x2c, 0xae, 0xf0,
	0x0e, 0xc4, 0x07, 0x5d, 0x7d, 0x2b, 0xb2, 0x20, 0x75, 0x33, 0x6b, 0x5b,
	0xf7, 0xe7, 0x17, 0x51, 0xf1, 0xab, 0xc1, 0x9e, 0xc6, 0xf0, 0x30, 0xc6,
	0x25, 0x26, 0x3e, 0xd7, 0xd7, 0xa3, 0xcc, 0x15, 0x95, 0x02, 0x03, 0x01,
	0x00, 0x01, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d,
	0x0e, 0x04, 0x16, 0x04, 0x14, 0x45, 0x8a, 0x76, 0xf7, 0x4f, 0xf4, 0x0e,
	0xa0, 0xf2, 0x02, 0xe1, 0xe7, 0xe9, 0xc7, 0x7d, 0x51, 0x55, 0x92, 0x33,
	0xcd, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16,
	0x80, 0x14, 0x45, 0x8a, 0x76, 0xf7, 0x4f, 0xf4, 0x0e, 0xa0, 0xf2, 0x02,
	0xe1, 0xe7, 0xe9, 0xc7, 0x7d, 0x51, 0x55, 0x92, 0x33, 0xcd, 0x30, 0x0c,
	0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff,
	0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
	0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x47, 0x93, 0x82, 0x0e,
	0x8a, 0x70, 0x9d, 0x6c, 0x7a, 0xdb, 0x04, 0xb4, 0xc9, 0xef, 0x98, 0x28,
	0xc6, 0xd9, 0x53, 0x90, 0xc8, 0x25, 0x83, 0x07, 0x23, 0xe7, 0x59, 0x38,
	0xc1, 0xc0, 0x50, 0x28, 0x99, 0x92, 0xfb, 0x21, 0x24, 0x72, 0xe5, 0xa6,
	0x57, 0x30, 0x31, 0xb3, 0xdf, 0xa0, 0x17, 0xa9, 0x73, 0x9c, 0x39, 0x83,
	0xfb, 0xe4, 0xfa, 0x20, 0x1d, 0xfa, 0x33, 0x20, 0x0c, 0x72, 0x2a, 0x50,
	0x40, 0xbd, 0x2d, 0x33, 0xa2, 0xfc, 0x06, 0xf9, 0xfe, 0x86, 0x4f, 0x50,
	0x1d, 0x65, 0x37, 0xe9, 0x30, 0x33, 0x82, 0xa1, 0x75, 0x8f, 0x5d, 0x33,
	0x84, 0x0d, 0xf2, 0x09, 0x04, 0xc0, 0x7a, 0x12, 0x79, 0xdb, 0x4f, 0x77,
	0x04, 0xe4, 0xd8, 0x0b, 0x87, 0x19, 0xba, 0xb7, 0x3c, 0xa6, 0x45, 0xaa,
	0x91, 0x62, 0x7f, 0x01, 0x7d, 0xc6, 0x20, 0x6d, 0x71, 0x15, 0x74, 0x5e,
	0x87, 0xb3, 0x60, 0x17, 0x9c, 0xc0, 0xed, 0x01, 0x4b, 0xb3, 0x23, 0x24,
	0xc1, 0xcb, 0x7a, 0x83, 0x03, 0x26, 0x2d, 0xde, 0x47, 0xc5, 0x11, 0x94,
	0x28, 0x27, 0x15, 0x92, 0x00, 0x8b, 0x2e, 0x51, 0x42, 0xca, 0x4b, 0x4a,
	0x2c, 0x51, 0x37, 0x56, 0xd0, 0xbc, 0x33, 0xd5, 0xd5, 0x3e, 0x79, 0x5c,
	0x3f, 0x9d, 0x6e, 0xb1, 0xe9, 0x71, 0xf1, 0x2c, 0xe9, 0xb4, 0x88, 0x2c,
	0xd2, 0x49, 0x97, 0xce, 0x29, 0x94, 0x16, 0xc9, 0xf9, 0x64, 0x0e, 0xd0,
	0xd9, 0x7a, 0x53, 0x10, 0x1a, 0xee, 0x83, 0x73, 0x93, 0x1b, 0xdf, 0x8a,
	0x77, 0xc0, 0x56, 0x63, 0xab, 0x5a, 0x65, 0xc5, 0xc5, 0x3b, 0xf3, 0x30,
	0x80, 0xfc, 0x38, 0x8b, 0xc9, 0xcd, 0xc3, 0x4f, 0x2e, 0x2d, 0x67, 0xcc,
	0x17, 0x18, 0x9b, 0x3e, 0xc6, 0x47, 0x03, 0xfc, 0x35, 0xa8, 0x35, 0x06,
	0x5a, 0x77, 0xe5, 0x97, 0x71, 0xbb, 0x27, 0x93, 0x0d, 0x1f, 0x0e, 0x8c,
	0x31, 0x82, 0x02, 0x9b, 0x30, 0x82, 0x02, 0x97, 0x02, 0x01, 0x01, 0x30,
	0x81, 0x87, 0x30, 0x7a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
	0x06, 0x13, 0x02, 0x4a, 0x50, 0x31, 0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55,
	0x04, 0x08, 0x0c, 0x05, 0x54, 0x6f, 0x6b, 0x79, 0x6f, 0x31, 0x0e, 0x30,
	0x0c, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x05, 0x54, 0x6f, 0x6b, 0x79,
	0x6f, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x06,
	0x4c, 0x69, 0x6e, 0x61, 0x72, 0x6f, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
	0x55, 0x04, 0x0b, 0x0c, 0x02, 0x53, 0x57, 0x31, 0x0f, 0x30, 0x0d, 0x06,
	0x03, 0x55, 0x04, 0x03, 0x0c, 0x06, 0x54, 0x65, 0x73, 0x74, 0x65, 0x72,
	0x31, 0x1c, 0x30, 0x1a, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
	0x01, 0x09, 0x01, 0x16, 0x0d, 0x74, 0x65, 0x73, 0x74, 0x40, 0x74, 0x65,
	0x73, 0x74, 0x2e, 0x6f, 0x72, 0x67, 0x02, 0x09, 0x00, 0xd7, 0x17, 0x0a,
	0x76, 0xd5, 0xd3, 0x4d, 0xeb, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48,
	0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0xa0, 0x81, 0xe5, 0x30,
	0x19, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x03,
	0x31, 0x0c, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x02,
	0x01, 0x04, 0x30, 0x1c, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
	0x01, 0x09, 0x05, 0x31, 0x0f, 0x17, 0x0d, 0x31, 0x39, 0x31, 0x30, 0x31,
	0x38, 0x30, 0x35, 0x35, 0x35, 0x32, 0x36, 0x5a, 0x30, 0x2f, 0x06, 0x09,
	0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x04, 0x31, 0x22, 0x04,
	0x20, 0x13, 0xe9, 0x2d, 0xcd, 0x35, 0x43, 0xe0, 0x13, 0x34, 0xc5, 0x67,
	0xde, 0xdd, 0x75, 0xdc, 0x62, 0x97, 0x76, 0x7d, 0x5b, 0xa0, 0xb4, 0x4d,
	0x4f, 0xef, 0xb8, 0xa7, 0x95, 0x50, 0xcb, 0x0f, 0xec, 0x30, 0x79, 0x06,
	0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x0f, 0x31, 0x6c,
	0x30, 0x6a, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03,
	0x04, 0x01, 0x2a, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65,
	0x03, 0x04, 0x01, 0x16, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
	0x65, 0x03, 0x04, 0x01, 0x02, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48,
	0x86, 0xf7, 0x0d, 0x03, 0x07, 0x30, 0x0e, 0x06, 0x08, 0x2a, 0x86, 0x48,
	0x86, 0xf7, 0x0d, 0x03, 0x02, 0x02, 0x02, 0x00, 0x80, 0x30, 0x0d, 0x06,
	0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x03, 0x02, 0x02, 0x01, 0x40,
	0x30, 0x07, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x07, 0x30, 0x0d, 0x06,
	0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x03, 0x02, 0x02, 0x01, 0x28,
	0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
	0x01, 0x05, 0x00, 0x04, 0x82, 0x01, 0x00, 0x38, 0x40, 0x09, 0xc7, 0xc4,
	0xf7, 0x78, 0x48, 0x75, 0x1e, 0xb2, 0x50, 0x95, 0x0a, 0x52, 0xee, 0x57,
	0x60, 0xc5, 0xf4, 0xdb, 0xca, 0x67, 0xb0, 0x19, 0xad, 0x68, 0xb1, 0xe1,
	0x1e, 0xb7, 0xf6, 0x53, 0x3d, 0x13, 0xb1, 0x11, 0x37, 0xa7, 0x6e, 0x9b,
	0x18, 0x1d, 0x0e, 0xbd, 0xc4, 0xb2, 0xd0, 0x36, 0x6c, 0x0c, 0x5a, 0x11,
	0x50, 0xcc, 0xdb, 0x1f, 0x6b, 0xcb, 0x28, 0x80, 0xd5, 0x3c, 0x4f, 0x93,
	0x0b, 0xd1, 0x45, 0x75, 0xa1, 0x89, 0x00, 0x71, 0x7d, 0x55, 0xcc, 0x1c,
	0x0a, 0xc9, 0xc4, 0xe6, 0x87, 0xf2, 0x87, 0x0d, 0x2e, 0x79, 0x71, 0x85,
	0x01, 0xd7, 0x32, 0x87, 0x9a, 0x11, 0xc6, 0x9a, 0xbb, 0x0a, 0x7b, 0xce,
	0xfe, 0xc8, 0xee, 0x10, 0x3c, 0xa6, 0x47, 0xdd, 0xbb, 0xa7, 0xf5, 0x19,
	0x50, 0xd5, 0x2a, 0x11, 0x44, 0x2f, 0x65, 0x09, 0x69, 0x50, 0xfa, 0xbd,
	0x02, 0xe4, 0x90, 0xdc, 0x2a, 0x7c, 0xdb, 0x82, 0x03, 0xa5, 0x28, 0x91,
	0x74, 0x7c, 0xd3, 0x83, 0xc8, 0x11, 0x1a, 0x14, 0x1b, 0xba, 0xb1, 0x82,
	0xbd, 0x53, 0xad, 0x9c, 0x34, 0x05, 0xfa, 0x2d, 0x14, 0x58, 0x5e, 0x50,
	0x64, 0x60, 0x5c, 0x21, 0x7c, 0xe6, 0xf0, 0x2b, 0xa2, 0xec, 0xe5, 0xeb,
	0xda, 0x88, 0xe2, 0x19, 0x36, 0x96, 0x65, 0xf7, 0x4c, 0x62, 0x9b, 0x75,
	0x24, 0xb4, 0xb1, 0x34, 0x83, 0xba, 0x05, 0x01, 0xd8, 0xe1, 0x33, 0xd3,
	0x1a, 0xd6, 0x09, 0x84, 0x31, 0xd0, 0x67, 0xf3, 0x3b, 0x0e, 0x19, 0x98,
	0x7e, 0x07, 0xdc, 0xe1, 0xd8, 0x45, 0x84, 0xa2, 0xdd, 0x8a, 0x04, 0x6a,
	0x43, 0xcf, 0xff, 0x7c, 0x9e, 0x83, 0xa8, 0x5d, 0xbc, 0x1f, 0x45, 0x86,
	0x5b, 0x2d, 0xcd, 0x9d, 0xa0, 0xba, 0x4d, 0xd2, 0xc6, 0xb9, 0xc5, 0x34,
	0x39, 0x29, 0x20, 0xee, 0x27, 0x60, 0x46, 0x9c, 0x62, 0xbe, 0xf2
};

static unsigned int image_pk7_len = 1811;

/**
 * lib_asn1_pkcs7() - unit test for asn1 decoder function
 * with pkcs7 message parser
 *
 * @uts:	unit test state
 * Return:	0 = success, 1 = failure
 */
static int lib_asn1_pkcs7(struct unit_test_state *uts)
{
	struct pkcs7_message *pkcs7;

	pkcs7 = pkcs7_parse_message(image_pk7, image_pk7_len);

	ut_assertf(!IS_ERR(pkcs7), "decoding failed\n");
	ut_assertf(pkcs7->data_len == 104, "signature size doesn't match\n");
	ut_assertf(pkcs7->signed_infos != NULL, "sign-info doesn't exist\n");
	ut_assertf(pkcs7->signed_infos->msgdigest_len == 32,
		   "digest size doesn't match\n");
	ut_assertf(pkcs7->signed_infos->aa_set == 0xf,
		   "authenticated attributes doesn't match\n");

	pkcs7_free_message(pkcs7);

	return CMD_RET_SUCCESS;
}

LIB_TEST(lib_asn1_pkcs7, 0);
#endif /* CONFIG_PKCS7_MESSAGE_PARSER */

#ifdef CONFIG_RSA_PUBLIC_KEY_PARSER
#include <crypto/internal/rsa.h>

/*
 * openssl genrsa 2048 -out private.pem
 * openssl rsa -in private.pem -pubout -outform der -out public.der
 * dd if=public.der of=public.raw bs=24 skip=1
 */
static const unsigned char public_key[] = {
	0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xca, 0x25, 0x23,
	0xe0, 0x0a, 0x4d, 0x8f, 0x56, 0xfc, 0xc9, 0x06, 0x4c, 0xcc, 0x94, 0x43,
	0xe0, 0x56, 0x44, 0x6e, 0x37, 0x54, 0x87, 0x12, 0x84, 0xf9, 0x07, 0x4f,
	0xe4, 0x23, 0x40, 0xc3, 0x43, 0x84, 0x37, 0x86, 0xd3, 0x9d, 0x95, 0x1c,
	0xe4, 0x8a, 0x66, 0x02, 0x09, 0xe2, 0x3d, 0xce, 0x2c, 0xc6, 0x02, 0x6a,
	0xd4, 0x65, 0x61, 0xff, 0x85, 0x6f, 0x88, 0x63, 0xba, 0x31, 0x62, 0x1e,
	0xb7, 0x95, 0xe9, 0x08, 0x3c, 0xe9, 0x35, 0xde, 0xfd, 0x65, 0x92, 0xb8,
	0x9e, 0x71, 0xa4, 0xcd, 0x47, 0xfd, 0x04, 0x26, 0xb9, 0x78, 0xbf, 0x05,
	0x0d, 0xfc, 0x00, 0x84, 0x08, 0xfc, 0xc4, 0x4b, 0xea, 0xf5, 0x97, 0x68,
	0x0d, 0x97, 0xd7, 0xff, 0x4f, 0x92, 0x82, 0xd7, 0xbb, 0xef, 0xb7, 0x67,
	0x8e, 0x72, 0x54, 0xe8, 0xc5, 0x9e, 0xfd, 0xd8, 0x38, 0xe9, 0xbe, 0x19,
	0x37, 0x5b, 0x36, 0x8b, 0xbf, 0x49, 0xa1, 0x59, 0x3a, 0x9d, 0xad, 0x92,
	0x08, 0x0b, 0xe3, 0xa4, 0xa4, 0x7d, 0xd3, 0x70, 0xc0, 0xb8, 0xfb, 0xc7,
	0xda, 0xd3, 0x19, 0x86, 0x37, 0x9a, 0xcd, 0xab, 0x30, 0x96, 0xab, 0xa4,
	0xa2, 0x31, 0xa0, 0x38, 0xfb, 0xbf, 0x85, 0xd3, 0x24, 0x39, 0xed, 0xbf,
	0xe1, 0x31, 0xed, 0x6c, 0x39, 0xc1, 0xe5, 0x05, 0x2e, 0x12, 0x30, 0x36,
	0x73, 0x5d, 0x62, 0xf3, 0x82, 0xaf, 0x38, 0xc8, 0xca, 0xfa, 0xa1, 0x99,
	0x57, 0x3c, 0xe1, 0xc1, 0x7b, 0x05, 0x0b, 0xcc, 0x2e, 0xa9, 0x10, 0xc8,
	0x68, 0xbd, 0x27, 0xb6, 0x19, 0x9c, 0xd2, 0xad, 0xb3, 0x1f, 0xca, 0x35,
	0x6e, 0x84, 0x23, 0xa1, 0xe9, 0xa4, 0x4c, 0xab, 0x19, 0x09, 0x79, 0x6e,
	0x3c, 0x7b, 0x74, 0xfc, 0x33, 0x05, 0xcf, 0xa4, 0x2e, 0xeb, 0x55, 0x60,
	0x05, 0xc7, 0xcf, 0x3f, 0x92, 0xac, 0x2d, 0x69, 0x0b, 0x19, 0x16, 0x79,
	0x75, 0x02, 0x03, 0x01, 0x00, 0x01
};

static unsigned int public_key_len = 270;

/**
 * lib_asn1_pkey() - unit test for asn1 decoder function
 * with RSA public key parser
 *
 * @uts:	unit test state
 * Return:	0 = success, 1 = failure
 */
static int lib_asn1_pkey(struct unit_test_state *uts)
{
	struct rsa_key pkey;
	int ret;

	ret = rsa_parse_pub_key(&pkey, public_key, public_key_len);

	ut_assertf(ret == 0, "decoding failed (%d)\n", ret);
	ut_assertf(pkey.n_sz == 257, "public key modulus size doesn't match\n");
	ut_assertf(pkey.e_sz == 3, "public key exponent size doesn't match\n");
	ut_assertf(pkey.e[0] == 0x01 && pkey.e[1] == 0x00 && pkey.e[2] == 0x01,
		   "public key exponent doesn't match\n");

	return CMD_RET_SUCCESS;
}

LIB_TEST(lib_asn1_pkey, 0);
#endif /* CONFIG_RSA_PUBLIC_KEY_PARSER */
