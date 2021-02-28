#!/bin/bash

F=al_hal_ddr_init_alpine_v1.c
cp al_hal_ddr_init.c $F

perl -i -pe  'BEGIN{undef $/;} s/CAS#/CAS^/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/Octet#/Octet^/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#define/^define/smg' $F

perl -i -pe  'BEGIN{undef $/;} s/#(if \([^A][^\n]*\n[^#]*)#endif\n/^\1^endif\n/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#(if \([^A][^\n]*\n[^#]*)#else\n([^#]*)#endif\n/^\1^else\n\2^endif\n/smg' $F

perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V1\)\n([^#]*)#endif\n/\1/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V3\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID > AL_DEV_ID_ALPINE_V1\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID > AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID <= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n/\1/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V3\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V1\)\n([^#]*)#elif \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n/\1/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V2\)\n([^#]*)#elif \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V3\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V1\)\n([^#]*)#elif \(AL_DEV_ID == AL_DEV_ID_ALPINE_V2\)\n([^#]*)#elif \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V3\)\n([^#]*)#endif\n/\1/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID <= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#elif \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V3\)\n([^#]*)#endif\n/\1/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID == AL_DEV_ID_ALPINE_V2\)\n([^#]*)#elif \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V3\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#else\n([^#]*)#endif\n/\2/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID <= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#else\n([^#]*)#endif\n/\1/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n//smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V3\)\n([^#]*)#else\n([^#]*)#endif\n/\2/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#else\n([^#]*)#endif\n/\2/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/#if \(AL_DEV_ID >= AL_DEV_ID_ALPINE_V2\)\n([^#]*)#endif\n//smg' $F

perl -i -pe  'BEGIN{undef $/;} s/CAS^/CAS#/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/Octet^/Octet#/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/\^define/#define/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/\^if/#if/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/\^else/#else/smg' $F
perl -i -pe  'BEGIN{undef $/;} s/\^endif/#endif/smg' $F

