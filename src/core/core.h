#ifndef CORE_H
#define CORE_H

// libs needed basically everywhere
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// controls debug vs. release behaviour
#define DEBUG

// null
#ifndef NULL
	#define NULL (void*)0
#endif

// bool
#ifndef bool
	#define bool int
#endif
#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif


// typedefs
typedef char			s8;
typedef short			s16;
typedef int			s32;
typedef long long		s64;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int 		u32;
typedef unsigned long long	u64;

typedef float 			f32;
typedef double			f64;


typedef void (empty_callback)(void);


#define INLINE static inline __attribute( (always_inline) )

#define FLAG(a) (1 << (a)) // aka. 2^a, 0, 2, 4, 8, ...

// a: 00010010
// b: 00000010
// &: 00000010 true bc. >1
#define HAS_FLAG(a, b)   ((a) & (b))

// print

#define STR_VAR(v)  (#v)
#define STR_BOOL(v) ((v) ? "true" : "false")
#define SPRINTF(max, ...)  ASSERT(sprintf(__VA_ARGS__) < (max)) 

#ifdef DEBUG

#define PF(...)		  printf(__VA_ARGS__)

#define P_INT(v) 	  PF("%s: %d\n", #v, v)
#define P_F32(v) 	  PF("%s: %f\n", #v, v)
#define P_STR(v) 	  PF("%s: \"%s\"\n", #v, v)
#define P_CHAR(v) 	PF("%s: %c\n", #v, (char)(v))
#define P_BOOL(v) 	PF("%s: %s\n", #v, STR_BOOL(v))

#define P(msg)		  PF("%s\n", msg)
#define P_IL(msg)	  PF("%s", msg)
#define P_TXT(txt)  PF("%s:\n%s\n", #txt, txt)

#define P_ERR(msg)	PF("[ERROR] %s\n -> file: %s, line: %d\n", msg, __FILE__, __LINE__)
#define P_ERRF(...)	PF("[ERROR] "); PF(__VA_ARGS__); PF(" -> file: %s, line: %d\n", __FILE__, __LINE__)
#define ASSERT(c) if(!(c)) { PF("[ASSERT] '%s'\n -> file: %s, line: %d\n", #c, __FILE__, __LINE__); abort(); }
#define ERR(msg)  P_ERR(msg); abort();
#define ERRF(...)  P_ERRF(__VA_ARGS__); abort();
#define ERR_CHECK(c, msg) if(!(c)) { ERR(msg); }
#define ERR_CHECKF(c, ...) if(!(c)) { ERRF(__VA_ARGS__); }

#elif

#define PF(...)		  
#define P_INT(v) 	  
#define P_F32(v) 	  
#define P_STR(v) 	  
#define P_CHAR(v) 	
#define P_BOOL(v) 	
#define P(msg)		  
#define P_IL(msg)	  
#define P_TXT(txt)  
#define P_ERR(msg)	
#define P_ERRF(...)	
#define ASSERT(c) 
#define ERR(msg)  
#define ERRF(...)  
#define ERR_CHECK(c, msg) 
#define ERR_CHECKF(c, ...) 

#endif


#endif
