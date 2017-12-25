#ifndef _UTILS_H
#define _UTILS_H

#define ReleaseCOM( x ) { if( x ){ x->Release( ); x = 0; } }

#define SafeDelete( x ) { delete x; x = 0; }

#endif