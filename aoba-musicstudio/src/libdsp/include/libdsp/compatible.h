#pragma once

#if __cplusplus >= 201703L
# define __FALLTHROUGH__ [[fallthrough]]
#else
# define __FALLTHROUGH__
#endif

