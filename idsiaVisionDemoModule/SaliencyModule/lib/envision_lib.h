//Header file for envision_lib...
//Richard Veale 15 may 2010
#include <opencv/cxcore.h>
#include "env_image.h"

#ifdef  __cplusplus
extern "C" {
#endif
  void envision_nextframe(const IplImage* ipl_input, IplImage* ipl_output, int eye_side);
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
extern "C" {
#endif
  int* SC_naive_competition(IplImage* ipl_outputL, IplImage* ipl_outputR); //returns 3 int array: eyeside, x, y
#ifdef  __cplusplus
}
#endif

/*#ifdef  __cplusplus
extern "C" {
#endif
  void SC_winners_update();
#ifdef  __cplusplus
}
#endif*/

#ifdef  __cplusplus
extern "C" {
#endif
  int* SC_winners_update();
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
extern "C" {
#endif
  void RF_inject(float current);
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
extern "C" {
#endif
  int SC_subset_winner(int* l_subset, int l_subset_l, int* r_subset, int r_subset_l); //assume l NUMBER OF POINTS (not size of array)
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
extern "C" {
#endif
  void SC_reset();
#ifdef  __cplusplus
}
#endif

#ifdef  __cplusplus
extern "C" {
#endif
  int envision_cleanup(void);
#ifdef  __cplusplus
}
#endif


//rev; old guy
#ifdef  __cplusplus
extern "C" {
#endif
  int* envision_nextpic_frommem(const IplImage* ipl_input, IplImage* output, int desired_numsalwinners, int eye_side);
#ifdef  __cplusplus
}
#endif
