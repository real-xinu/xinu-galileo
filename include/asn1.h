#ifndef ASN1
#define ASN1

/* class of tag */
#define UNIVERSAL   (0x00)
#define APPLICATION (0x40)
#define CONTEXTSPEC (0x80)
#define PRIVATE     (0xa0)

#define PRIMITIVE   (0x00)
#define CONSTRUCTED (0x20)

#define DEFINITE    (0x00)
#define UNDEFINITE  (0x01)

int32 asn1read_identifier(uint32* idp, byte* classp, byte* pcp, byte* src);
int32 asn1read_length(uint32* lengthp, byte* src);
int32 asn1is_end_content(byte* src);

int32 asn1write_identifier(byte* dst, uint32 id, byte class, byte pc);
int32 asn1write_length(byte* dst, uint32 length, byte form);
int32 asn1write_endcontent(byte* dst, byte form);

void launch_asn1tests();

#endif
