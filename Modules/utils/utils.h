#ifndef _UTILS_H_
#define _UTILS_H_

#ifndef TRUE
#define TRUE                (1 == 1)
#endif  /* TRUE */

#ifndef FALSE
#define FALSE               (!TRUE)
#endif  /* FALSE*/

#define IS_NULLPTR(ptr)     ((ptr) == NULL)
#define IS_ZERO(val)        ((val) == 0)

#define MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define MAX(a, b)           (((a) > (b)) ? (a) : (b))

#endif /* _UTILS_H_ */
