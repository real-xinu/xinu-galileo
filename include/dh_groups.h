typedef struct dh_groupmp {
	MPPRIME	p;
	uint32	generator;
	MPINT	order;
	int32	size;
} DH_GROUPMP;

extern DH_GROUPMP DH_G1;
extern DH_GROUPMP DH_G14;
