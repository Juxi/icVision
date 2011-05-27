//REV: Jan 2010, saliency map implementation/functions for running it in real time.
//REV: May 2011: modifications for running on binocular (L eye, R eye), and stuff for im-clever 2011

//REV: TODO make it so that when it chooses subset points do the gaussian around it (or even just mass)


//########## DEFINES ###########//

#define PRINT_SC 0

//instantaneous map Gaussian LPF SD (in pixels, 1 pixel == 1 unit distance). using salience "mass"
#define GAUSS_LPF_SD 0.5 //0.00001 for original one (i.e. no gaussian combination)
//instantaneous map Gaussian LPF cutoff value (in weight)
#define GAUSS_LPF_CUTOFF 0.1
//define to turn on/off edge correction for gaussian (basically normalize by integral of weights)
#define EDGECORR

#define SC_thresh 0.3 //arbitrary threshold
#define SC_W_mult 10.0

#define MEMB_T_CONST 0.05 //time constant for membrane potential of SC neurons

#define L_EYE 0
#define R_EYE 1
//##############################//




#include "env_alloc.h"
#include "env_c_math_ops.h"
#include "env_image.h"
#include "env_image_ops.h"
#include "env_log.h"
#include "env_mt_visual_cortex.h"
#include "env_params.h"
#include "env_pthread_interface.h"
#include "env_stdio_interface.h"
#include "env_visual_cortex.h"

#include <stdio.h>
#include <stdlib.h> // for atoi(), malloc(), free()
#include <math.h>
#include "envision_lib.h"
#include "opencv/cxcore.h"//this has the IplImage definition in it...
#include <sys/time.h>
#include <gsl/gsl_randist.h> //for gamma random
#include <gsl/gsl_rng.h> //gsl random number generator


int reset_SC();
float gaussian_val(float, float, float, float);
int x_loc(int, int);
int y_loc(int, int);
int x_y_to_array_loc(int, int, int);
static void* malloc_thunk(env_size_t);

gsl_rng* gsl_rand_source; //for uniform random

//========= SALMAP GAUSSIAN FILTER VARIABLES ============//
//   REV: LPF (via separable gauss filter) the INPUT to the long-term map (SC)
float** gauss_filter;
int gauss_filter_radius;
int gauss_filter_w;
int gauss_filter_c;

float** lpf_salmap;
int salmap_w; //width and height of output array (instantaneous salmap)
int salmap_h;

//========== SUPERIOR COLLICULUS VARIABLES ===========//
//   REV; note it's really just the salmap, and likely isn't in SC anyways. But whatever, they want something to work :)
float** SC_l; //left superior colliculus (long term salmap). These (should) be neurons, this is just float Vm
float** SC_r; //right superior colliculus (long term salmap)
float SC_memb_t_const=-1; //time constant of decay for membrane potential of superior colliculus neurons
float SC_memb_decay=-1; //analytically solved decay constant for remainder&leakage term of differential equation solution
float SC_memb_decay_compl=-1; //analytically solved decay constant for addition term of differential equation solution

//   --- TOP DOWN SC INHIB WEIGHTS from reticular formation RF---  //
//   REV: note supposed to be "interest" or "excitement" or "baseline arousal" as a result of stimulating stimulus
float** RF_SC_inhib; //inhib weight matrix. Note normally its not to SC but rather to e.g. BG or something.
float RF_gauss_SD = 1.5;
float RF_amplitude = 6.0;
float RF_w_baseline = 1.0; //baseline of weights ('lift off ground')

float RF; //reticular formation (population coding of excitement...?). This will be SET to the value, and decay exponent
float RF_memb_t_const = 1000; //0.5; //50ms...? //note it's dt DIV by this number.
float RF_decay;
//float RF_decay_compl; //don't need bc we're just going to "SET" once. Note, why not just do linear decay for RF?
float RF_baseline = 0.0; //baseline of activity in RF at all times... (+ gaussian?)

//  --- Substantia Nigra pars reticulata (SNr) GAUSSIAN NOISE WEIGHTS ----   // 
//REV: note this isn't modeling SNr effect, but rather DEVIATIONS in SNr effect. I.e. it's already doing top-down inhibition.
//(alternatively, try roulette wheel from prob mass)
float SNr_baseline = 1.0; //this is offset (zero?) or is it baseline of VALUE (i.e. for certain X, how much A do we get out?)
float SNr_gauss_SD = 0.1; //1.0; //this is for distribution TO DRAW FROM! Just need SD and mean (zero?)
float SNr_amplitude = 1.0; //multiply things by? Prob of getting 'really far out x values' is less!

//NOTE TO SELF: using learning RATE, not learning PROGRESS (?)

float SC_l_maxval=-1;
float SC_r_maxval=-1;
int SC_l_maxx=-1;
int SC_l_maxy=-1;
int SC_r_maxx=-1;
int SC_r_maxy=-1;

int SC_win_side = -1;//0 for left eye (then look to left max) //1 for right eye (then look to right max)
//or just choose the subset if we get sent a subset...right?


//========== VARIABLES SO WE DONT GET SEGFAULTS FROM LACK OF INITIALIZATION =======//
int INITIALIZED = 0; //initially, nothing is initialized
int FIRSTFRAMER = 1; //initially, it's the first frame for R and L side
int FIRSTFRAMEL = 1;
int divnum=1; //divnum for division from full input image size to output image size





// ######################################################################
struct status_data
{
  int frame_number;
};

//REV: some globals...just for now
struct env_params envp; //REV: hack for left and right eye!
struct env_visual_cortex ivcR; //REV: this is holding each previous img (lowpass5 filtered) for motion (one for each eye)
struct env_visual_cortex ivcL;
env_size_t npixels = 0;
int multithreaded=0;


void envision_init(int w, int h) //just do both the eyes!
{
  printf("INITIALIZING...");
  
  //random stuff init, GSL (gnu scientific library) (for gaussians, etc., for when we have a tie!)
  long rand_seed = time(NULL); //errr...is this always same? :D
  gsl_rand_source = gsl_rng_alloc(gsl_rng_taus);
  gsl_rng_set(gsl_rand_source, rand_seed);
  
   //------ INITIALIZING NORMAL STUFF -----------------------//
  //Instantiate our various ModelComponents:
  env_params_set_defaults(&envp);
  
  envp.maxnorm_type = ENV_VCXNORM_MAXNORM;
  //envp.maxnorm_type = ENV_VCXNORM_NONE;
  
  
  envp.scale_bits = 16; //REV: does this relate to the output level?
  
  env_assert_set_handler(&env_stdio_assert_handler);
  if (multithreaded)
    {
      env_init_pthread_alloc();
      env_init_pthread_job_server();
    }
  env_allocation_init(&malloc_thunk, &free);
    
  //LEFT SIDE
  env_visual_cortex_init(&ivcL, &envp);
  env_visual_cortex_init(&ivcR, &envp);
  printf("FINISHED SALIENCY MAP INITIALIZATION\n\n");
  
  //--------------------- INIT LPF AND SETTING SOME IMPORTNANT W AND H-----------//
  divnum=1; for(int x=0; x<envp.output_map_level; x++) { divnum *= 2; } //just doing 2^output_map_level
  salmap_w = w/divnum;
  salmap_h = h/divnum; //not even...?
  
  lpf_salmap = malloc(sizeof(float*) * salmap_w);
  for(int x=0; x<salmap_w; x++)
    {
      lpf_salmap[x] = malloc(sizeof(float) * salmap_h);
    }

  //----------------------- INITIALIZING GAUSSIAN FILTER ---------------------//
  //init gaussian filter for massing pixels. Note biggest x or y will be at axes.
  gauss_filter_radius = 0;
  
  printf("Calculating 2-dimensional gaussian low-pass-filter\n");
  
  //basically a do-while loop
  float val=gaussian_val((float)gauss_filter_radius, GAUSS_LPF_SD, 0, 1); //sd, mean, scale
  while(val >= GAUSS_LPF_CUTOFF)
    {
      printf("%0.3f ", val);
      ++gauss_filter_radius;
      val= gaussian_val((float)gauss_filter_radius, GAUSS_LPF_SD, 0, 1); //sd, mean, scale
      
    }
  printf(" --  (radius = %i)\n\n", gauss_filter_radius); //note this is the radius i.e. LESS THAN value, we'd do x=0; x<RAD; x++
  gauss_filter_w = gauss_filter_radius*2-1;
  gauss_filter_c = gauss_filter_radius-1;
  
  //SEE: LPF SALMAP INIT IN FIRST PASS THROUGH NEXTFRAME
  
  gauss_filter = malloc(sizeof(float*) * gauss_filter_w);
  for(int x=0; x<gauss_filter_w; x++)
    {
      gauss_filter[x] = malloc(sizeof(float) * gauss_filter_w);
      for(int z=0; z<gauss_filter_w; z++)
	{
	  //dist is x and z, i.e. sqrt (x^2 + z^2)
	  int xdist = x-gauss_filter_c;
	  int zdist = z-gauss_filter_c;
	  float dist = sqrt(xdist*xdist + zdist*zdist);
	  if(dist <= gauss_filter_radius)
	    gauss_filter[x][z] = gaussian_val(dist, GAUSS_LPF_SD, 0, 1);
	  else
	    gauss_filter[x][z] = 0;
	  
	  //gauss_filter[x][z] = sqrt(xdist*xdist + zdist*zdist);
	  printf("%4.3f ", gauss_filter[x][z]);
	}
      printf("\n");
    }
  printf("\n");
  
  printf("FINISHED INITIALIZING GAUSSIAN FILTERS\n");
  
  //----- INITIALIZING SUPERIOR COLLICULUS SIMULATIONS -------//
  SC_memb_t_const = MEMB_T_CONST; //in s, i.e. 500ms
  SC_memb_decay = exp(-SC_memb_t_const); //div DT
  SC_memb_decay_compl = 1 - SC_memb_decay;
  SC_l = malloc(sizeof(float*) * salmap_w); //assume we just want salmap w neurons. note we could just use fixed memory...
  SC_r = malloc(sizeof(float*) * salmap_w);
  for(int x=0; x<salmap_w; x++)
    {
      SC_l[x] = malloc(sizeof(float) * salmap_h);
      SC_r[x] = malloc(sizeof(float) * salmap_h);
      for(int y=0; y<salmap_h; y++)
	{
	  SC_l[x][y] = 0; //set all membrane potentials to 0mV...
	  SC_r[x][y] = 0;
	}
    }
  
  printf("FINISHED INITIALIZING SUPERIOR COLLICULUS MODEL\n");
  
  // ------ INITIALIZE RETICULAR FORMATION DYNAMICS (TC etc.)---//
  RF_decay = exp(-RF_memb_t_const);
  RF = RF_baseline; //remember to reset every time (SC_reset())

  
  // ------ INITIALIZE RETICULAR FORMATION WEIGHTS ------------//
  RF_SC_inhib = malloc(sizeof(float*) * salmap_w);
  for(int x=0; x<salmap_w; x++)
    RF_SC_inhib[x] = malloc(sizeof(float) * salmap_h);
  
  float RF_w_c = (float)(salmap_w-1)/2; //just assume circle macula (fovea)
  float RF_h_c = (float)(salmap_h-1)/2;
  printf("Reticular Formation -> Superior Colliuclus (RF->SC) weights (inhib): (centre: %f %f)\n", RF_w_c, RF_h_c);
  for(int y=0; y<salmap_h; y++)
    {
      for(int x=0; x<salmap_w; x++)
	{
	  float dist = sqrt((((float)x-RF_w_c)*((float)x-RF_w_c))+(((float)y-RF_h_c)*((float)y-RF_h_c)));
	  float w = -RF_w_baseline + -gaussian_val(dist, RF_gauss_SD, 0.0, RF_amplitude);
	  RF_SC_inhib[x][y] = w;
	  printf("%2.2f ", w);
	}
      printf("\n");
    }
  printf("FINISHED INITIALIZING RF->SC weights\n\n");
 
  
  // ------ INIT GSL RANDOM STUFF FOR SNr afferents' GAUSSIAN NOISE ------//
  //done (at top)
  

} //end envision_init


//======================= ENVISION NEXTPIC FROMMEM ===============================//

void envision_nextframe(const IplImage* ipl_input,  //ipl image as input
		       IplImage* ipl_output, int eye_side)   //output image we'll write to (use ->imageData to write)
{
    
  struct env_dims indims;
  indims.w = ipl_input->width;
  indims.h = ipl_input->height;
  
  //input is ipl_input as const env_rgb_pixel*
  const struct env_rgb_pixel* input = (struct env_rgb_pixel*)ipl_input->imageData;
  
  npixels = indims.w * indims.h;
    
  //initialize if necessary...note size is pretty manual. We really want ivcout.dims.w etc.
  if(!(INITIALIZED == 1))
    {
      envision_init(indims.w, indims.h);
      
      
      INITIALIZED=1;
    }
    
  struct env_image ivcout = env_img_initializer;
  struct env_image intens = env_img_initializer;
  struct env_image color = env_img_initializer;
  struct env_image ori = env_img_initializer;
#ifdef ENV_WITH_DYNAMIC_CHANNELS
  struct env_image flicker = env_img_initializer;
  struct env_image motion = env_img_initializer;
#endif

  struct status_data userdata; //REV: need this?
  //used to have userdata.frame_number = img_num
  
  if(eye_side == L_EYE)
    {
      //REV: note this function will call env_visual_cortex_input (the non-multithreaded version) since it's not multithreaded now.
      env_mt_visual_cortex_input(multithreaded,
				 &ivcL, &envp,
				 "visualcortex",
				 input, 0, indims,
				 0, /*&print_chan_status, //REV: want this?*/ //we don't need status_func...if we don't
				 //want to be printing shit.
				 &userdata,
				 &ivcout,
				 &intens, &color,
				 &ori
#ifdef ENV_WITH_DYNAMIC_CHANNELS
				 , &flicker, &motion
#endif
				 );
  
    } //end if eyeside == 0
  else if(eye_side == R_EYE)
    {  
      //REV: note this function will call env_visual_cortex_input (the non-multithreaded version) since it's not multithreaded now.
      env_mt_visual_cortex_input(multithreaded,
				 &ivcR, &envp,
				 "visualcortex",
				 input, 0, indims,
				 0, /*&print_chan_status, //REV: want this?*/ //we don't need status_func...if we don't
				 //want to be printing shit.
				 &userdata,
				 &ivcout,
				 &intens, &color,
				 &ori
#ifdef ENV_WITH_DYNAMIC_CHANNELS
				 , &flicker, &motion
#endif
				 );
    } //end if eyeside == R_EYE
  
  
  //--------------(things to do only after first frame)------------//
  if((eye_side == L_EYE && FIRSTFRAMEL == 0) || (eye_side == R_EYE && FIRSTFRAMER == 0)) 
    {
      //REV: still not sure that passing over rescale step is OK, but trying it anyways.
      //values are between 0 and INT_MAX (highest no guarantee might
      //be low), instead of 0, CHAR_MAX (highest guarneteed charmax)
      /*
      env_visual_cortex_rescale_ranges(&ivcout, &intens, &color, &ori
#ifdef ENV_WITH_DYNAMIC_CHANNELS
				       , &flicker, &motion
#endif
				       );
      */
      
      //------------ Do stuff in here -------------//
      
      //CALCULATE LPF SALMAP FOR THIS INPUT (no timecourse? Whatever. Make gaussian sufficiently small and we get basically nothing)      
      for(int y=0; y<salmap_h; y++)
	{
	  for(int x=0; x<salmap_w; x++)
	    {
	      lpf_salmap[x][y]=0;
#ifdef EDGECORR
	      float integral=0;
#endif
	      for(int dx=0; dx<gauss_filter_w; dx++)
		{
		  for(int dy=0; dy<gauss_filter_w; dy++)
		    {
		      //dx, dy is location in gauss_filter
		      //but actual location is x+(dx-gauss_filter_c). I.e. first is x + (0-4), i.e. x-4. Last is x+(8-4) = x+4
		      int tx= x+(dx-gauss_filter_c); // ("true" x, true y, etc.)
		      int ty= y+(dy-gauss_filter_c);
		      //printf("Calc true xy: (%2i, %2i)\n", tx, ty);
		      if(tx >= 0 && tx < salmap_w && ty >= 0 && ty < salmap_h) //only if legal point
			{
#ifdef EDGECORR
			  integral += gauss_filter[dx][dy];
#endif
			  int loc = x_y_to_array_loc(tx, ty, salmap_w);
			  float val = (float)ivcout.pixels[loc] / (float)INT_MAX;
			  lpf_salmap[x][y] += gauss_filter[dx][dy] * val;
			}
		    }
		}
	      
	      //int val = ivcout.pixels[x_y_to_array_loc(x, y, salmap_w)];
	      //printf("%0.3f ", (float)val/(float)INT_MAX);
	      
#ifdef EDGECORR
	      lpf_salmap[x][y] /= integral;
#endif
	      //printf("%0.3f ", lpf_salmap[x][y]);
	      //printf("%0.3f ", integral);
	      
	    }
	  //printf("\n");
	}
      //printf("\n");
      
      
      
      //we now have stored as floats in lpf_salmap[x][y] the lpf version of the salmap.
      //Now, use this to update the appropriate long-term saliency map's individual neurons
      
      
      //REV: TODO UPDATE LONG-TERM SALIENCY MAP HERE
      //SC_l and SC_r using SC_memb_decay and SC_memb_decay_compl. Do I need to calculate Isyn better?
      //feed current lpf_salmap[x][y] as input into each SC_l[x][y] neuron

      //note, we could've done the gaussian at THIS level as well (i.e. a weight array into each neuron that samples
      //from the surround of the next level down). It's all going to sum anyways, just doing it pre-transmission is good, since linear syn.
      
      if(eye_side == L_EYE)
	SC_l_maxval = -1;
      else if(eye_side == R_EYE)
	SC_r_maxval = -1;
      
      for(int x=0; x<salmap_w; x++)
	{
	  for(int y=0; y<salmap_h; y++)
	    {
	      //MAKE SURE TO ONLY UPDATE RF ONCE!
	      float inhib = RF * RF_SC_inhib[x][y];
	      float noise = gsl_ran_gaussian(gsl_rand_source, SNr_gauss_SD);
	      float Isyn = (lpf_salmap[x][y] * SC_W_mult) + noise + inhib;
	      //printf("Adding: %0.2f (decay: %0.2f   compl: %0.2f)\n", Isyn, SC_memb_decay, SC_memb_decay_compl);
	      
	      if(eye_side == L_EYE)
		{
		  
		  SC_l[x][y] = (SC_l[x][y] * SC_memb_decay) + (SC_memb_decay_compl * (0+Isyn)); //0 is Ibg
		  //REV: moving these check maxval things down to where we do naive competition
		  /*if(SC_l[x][y] > SC_l_maxval)
		    {
		      SC_l_maxval = SC_l[x][y];
		      SC_l_maxx= x;
		      SC_l_maxy= y;
		      
		      }*/
		  
		}
	      else if(eye_side == R_EYE)
		{
		  SC_r[x][y] = (SC_r[x][y] * SC_memb_decay) + (SC_memb_decay_compl * (0+Isyn)); //0 is Ibg
		  /*if(SC_r[x][y] > SC_r_maxval)
		    {
		      SC_r_maxval = SC_r[x][y];
		      SC_r_maxx= x;
		      SC_r_maxy= y;
		      }*/
		}
	    }
	}
      
      
      //DRAW STUFF (if you want to draw the individual saliency channels)
      
      // cleaning up memory (deallocating the different salmaps channels & ivcout!) for next run-through
      env_img_make_empty(&ivcout);
      env_img_make_empty(&intens);
      env_img_make_empty(&color);
      env_img_make_empty(&ori);
#ifdef ENV_WITH_DYNAMIC_CHANNELS
      env_img_make_empty(&flicker);
      env_img_make_empty(&motion);
#endif
    } //end if eye_side==L_EYE && FIRSTFRAMEL == 0
  
  //REV: weve been through things once, set FIRSTFRAMEL/R to false
  if(eye_side == L_EYE)       FIRSTFRAMEL=0; 
  else if(eye_side == R_EYE)  FIRSTFRAMER=0;
  
} //end nextpic_frommem


//=================== SC_FIND_WINNERS_SUBSET ==================//
int SC_subset_winner(int* l_subset, int l_subset_l, int* r_subset, int r_subset_l) //assume l NUMBER OF POINTS (not size of array)
{
    
  SC_l_maxval = -1;
  SC_r_maxval = -1;
  
  //naive fashion: just find the max of the subsets given to us.
  
  int lwin=-1;
  int rwin=-1;
  
  //LEFT SIDE
  for(int x=0; x<l_subset_l; x+=2)
    {
      int xloc = l_subset[x] / 16; //have to div by 16
      int yloc = l_subset[x+1] / 16; //have to div by 16
      float val = SC_l[xloc][yloc];
      if(val > SC_l_maxval) // && greater than threshold?
	{
	  lwin = x/2; //ARRAY INDEX
	  SC_l_maxval = val;
	  SC_l_maxx = xloc;
	  SC_l_maxy = yloc;
	}
    }
  //RIGHT SIDE
  for(int x=0; x<r_subset_l; x+=2)
    {
      int xloc = r_subset[x] /16;
      int yloc = r_subset[x+1] /16;
      float val = SC_r[xloc][yloc];
      if(val > SC_r_maxval) // && greater than threshold?
	{
	  rwin = x/2; //ARRAY INDEX
	  SC_r_maxval = val;
	  SC_r_maxx = xloc;
	  SC_r_maxy = yloc;
	}
    }
 
  if(SC_l_maxval > SC_r_maxval && SC_l_maxval > -1)
    {
      return lwin;
    }
  if(SC_r_maxval > SC_l_maxval && SC_r_maxval > -1)
    {
      return rwin;
    }
  else
    return -1;
} //end subset

//==================== SC_WINNERS_UPDATE ============//
/*void SC_winners_update()
{
  SC_l_maxval = -1;
  SC_r_maxval = -1;
  
  for(int x=0; x<salmap_w; x++)
    {
      for(int y=0; y<salmap_h; y++)
	{
	  //LEFT SIDE
	  if(SC_l[x][y] > SC_l_maxval)
	    {
	      SC_l_maxval = SC_l[x][y];
	      SC_l_maxx= x;
	      SC_l_maxy= y;
	      
	    }
	  //RIGHT SIDE
	  if(SC_r[x][y] > SC_r_maxval)
	    {
	      SC_r_maxval = SC_r[x][y];
	      SC_r_maxx= x;
	      SC_r_maxy= y;
	    }
	}
    }
    }*/

//==================== SC_WINNERS_UPDATE (REV 18 May 2011 -- with return!) ============//
int* SC_winners_update() 
{
  SC_l_maxval = -1;
  SC_r_maxval = -1;
  SC_l_maxx = -1; SC_l_maxy = -1;
  SC_r_maxx = -1; SC_r_maxy = -1;
  
  for(int x=0; x<salmap_w; x++)
    {
      for(int y=0; y<salmap_h; y++)
	{
	  //LEFT SIDE
	  if(SC_l[x][y] > SC_l_maxval)
	    {
	      SC_l_maxval = SC_l[x][y];
	      SC_l_maxx= x;
	      SC_l_maxy= y;
	      
	      
	    }
	  //RIGHT SIDE
	  if(SC_r[x][y] > SC_r_maxval)
	    {
	      SC_r_maxval = SC_r[x][y];
	      SC_r_maxx= x;
	      SC_r_maxy= y;
	      
	    }
	}
    }
  int* rval = malloc(sizeof(int) * 4); //xy winner of x and y -- note it isn't doing thresholding yet!!!! note some of these might be -1...?
  
  rval[0] = SC_l_maxx;
  rval[1] = SC_l_maxy;
  rval[2] = SC_r_maxx;
  rval[3] = SC_r_maxy;
  //printf("IN ENVLIB: Lwin: %i %i  Rwin: %i %i\n", rval[0], rval[1], rval[2], rval[3]);
  
  return rval;
}

//================== RF INJECT ========================//
void RF_inject(float excitement)
{
  RF = excitement; //or RF = excitement?
}


//REV: this doesn't deal with ties yet (or sub-threshold stuff!). Add constant BG currents to FORCE SC neurons to
//start firing if nothing happens (e.g. if in dark!) But, input might beat out the BG currents. The currents will force
//one to cross threshold. But, we want a sort of probabilistic thing?

//==================== SC_NAIVE_COMPETITION =====================//
//compares SC_l_maxval to SC_r_maxval (req. maxval > 1) and gives a winner. Draws?
int* SC_naive_competition(IplImage* ipl_outputL, IplImage* ipl_outputR)
{
  //do RF decay here!
  RF = RF * RF_decay;
  
  int* SC_winner = malloc(sizeof(int) * 3); //return 3. EYE_SIDE (-1 is none, 0 is left, 1 is right), then X, Y
  SC_winner[0] = -1;
  
  float maxval=-1;
  if(SC_l_maxval > maxval)
    {
      maxval = SC_l_maxval;
      if(maxval > SC_thresh)
	SC_win_side = L_EYE;
    }
  if(SC_r_maxval > maxval)
    {
      maxval = SC_r_maxval;
      if(maxval>SC_thresh)
	SC_win_side = R_EYE;
    }
  //else, maxval just stays -1, and winside remains -1 (i.e. no winner (yet))
  //we might want a THRESHOLD for winning too! (the other way of doing things)
  
  if(PRINT_SC)
    {
  if(SC_win_side == L_EYE)
    printf("WIN (L EYE): %0.2f (%i %i)\n", SC_l_maxval, SC_l_maxx, SC_l_maxy);
  else if(SC_win_side == R_EYE)
    printf("WIN (R EYE): %0.2f (%i %i)\n", SC_r_maxval, SC_r_maxx, SC_r_maxy);
  else
    printf("WIN: NONE\n");
  
  for(int y=0; y<salmap_h; y++)
    {
      for(int x=0; x<salmap_w; x++)
	{
	  printf("%0.1f ", SC_l[x][y]);
	}
      
      /*printf("     ");
      for(int x=0; x<salmap_w; x++)
	{
	  printf("%0.1f ", SC_r[x][y]);
	    }
      */
      printf("\n");
    }
  printf("\n\n");
    }   
  //either way, draw the shizzle! //first, just draw the two!
  //just draw at original size! (do L first, then R)
  //problem is that the LIF neurons have no "cap" for their Vm, since they could explode from lots of input
  //(unless we force them to hit a cap and stay there...?). But, we can't always just scale (though we could do first-to threshold!)
  //so just do everything that's at threshold as "white" and at continuous levels beneath it.
  
  //so, 0 is 0.0, 255 is SC_thresh (and above).    --> (val/thresh) (cap to 1) then mult by 255.
  //Also, if what-you-call it is on this side (& above thresh, but that's implicit), draw a (yellow) circle!
  
  int width = ipl_outputL->width;
  int height = ipl_outputL->height;
  char* img_output_ptr = ipl_outputL->imageData;
  float** state_ptr = SC_l;
  
  float CAP=SC_thresh;
  //find max first...lame
  for(int x=0; x<salmap_w; x++)
    {
      for(int y=0; y<salmap_h; y++)
	{
	  if(SC_l[x][y] > CAP)
	    CAP = SC_l[x][y];
	  if(SC_r[x][y] > CAP)
	    CAP = SC_r[x][y];
	}
    }
  
  //for each state_ptr[x][y], go and draw a 16x16 square in output (starting at x*16, y*16), offset by proper amount.
  for(int x=0; x<salmap_w; x++) //16 is hard coded lewl
    {
      for(int y=0; y<salmap_h; y++)
	{
	  float val = state_ptr[x][y] / CAP; //calc ratio
	  if(val > 1) { printf("SOMETHING IS BROKEN IT SHOULDNT BE BIGGER THAN 1 LEFT\n"); exit(1);}
	  if(val < 0) val = 0;
	  val *= 255; //put it in right range
	  int intval = (int)val; //this SHOULD be between 0, 255... 
	  char charval = (char)intval; //(converting directly might do something fishy with floating point representation)
	  for(int dy=0; dy<16; dy++)
	    {
	      for(int dx=0; dx<16; dx++)
		{
		  
		  int corrx = x*16 + dx;
		  int corry = y*16 + dy;
		  //printf("Writing to: %i %i\n", corrx, corry);
		  int offsetloc = x_y_to_array_loc(corrx, corry, width)*3;
		  img_output_ptr[offsetloc+0] = charval;
		  img_output_ptr[offsetloc+1] = charval;
		  img_output_ptr[offsetloc+2] = charval;
		}
	    }
	}
    }
  
  if(SC_win_side == L_EYE)
    {
      //draw circle
      //SC_l_maxx, SC_l_maxy 
      //draw circle
      //SC_r_maxx, SC_r_maxy
      int realx = SC_l_maxx*16 + 8; //8 to put in mid of winner square
      int realy = SC_l_maxy*16 + 8;
      //draw a circle around that. Circle radius is gauss_filter_radius
      int realrad = gauss_filter_radius*16;
      for(int y= realy - realrad; y<realy+realrad; y++)
	{
	  for(int x=realx -realrad; x<realx+realrad; x++)
	    {
	      float dist = sqrt((x-realx) * (x-realx)+ (y-realy)*(y-realy));
	      if(dist <= realrad && dist >= realrad-1 && y >= 0 && y<height && x>=0 && x<width)
		{
		  int offsetloc = x_y_to_array_loc(x, y, width)*3;
		  img_output_ptr[offsetloc+0] = 0;
		  img_output_ptr[offsetloc+1] = 255;
		  img_output_ptr[offsetloc+2] = 255;
		}
	  
	    }
	}
    }
  
  
  //RIGHT SIDE
  img_output_ptr = ipl_outputR->imageData;
  state_ptr = SC_r;
  
  //for each state_ptr[x][y], go and draw a 16x16 square in output (starting at x*16, y*16), offset by proper amount.
  for(int x=0; x<salmap_w; x++) //16 is hard coded lewl
    {
      for(int y=0; y<salmap_h; y++)
	{
	  float val = state_ptr[x][y] / CAP; //calc ratio
	  if(val > 1) { printf("SOMETHING IS BROKEN IT SHOULDNT BE GREATHER THAN 1 RIGHT\n"); exit(1);}//cap it
	  if(val < 0) val = 0;
	  val *= 255; //put it in right range
	  int intval = (int)val; //this SHOULD be between 0, 255... 
	  char charval = (char)intval; //(converting directly might do something fishy with floating point representation)
	  for(int dx=0; dx<16; dx++)
	    {
	      for(int dy=0; dy<16; dy++)
		{
		  int corrx = x*16 + dx;
		  int corry = y*16 + dy;
		  int offsetloc = x_y_to_array_loc(corrx, corry, width)*3;
		  img_output_ptr[offsetloc+0] = charval;
		  img_output_ptr[offsetloc+1] = charval;
		  img_output_ptr[offsetloc+2] = charval;
		}
	    }
	}
    }//end for all x in salmap_w

  if(SC_win_side == R_EYE)
    {
      //draw circle
      //SC_r_maxx, SC_r_maxy
      int realx = SC_r_maxx*16 + 8; //8 to put it in middle of winner square
      int realy = SC_r_maxy*16 + 8;
      //draw a circle around that. Circle radius is gauss_filter_radius
      int realrad = gauss_filter_radius*16;
      for(int y= realy - realrad; y<realy+realrad; y++)
	{
	  for(int x=realx -realrad; x<realx+realrad; x++)
	    {
	      float dist = sqrt((x-realx) * (x-realx) + (y-realy)*(y-realy));
	      if(dist <= realrad && dist >= realrad-1 && y >= 0 && y<height && x>=0 && x<width)
		{
		  int offsetloc = x_y_to_array_loc(x, y, width)*3;
		  img_output_ptr[offsetloc+0] = 0;
		  img_output_ptr[offsetloc+1] = 255;
		  img_output_ptr[offsetloc+2] = 255;
		}
	  
	    }
	}
      
    }
  
  SC_winner[0] = SC_win_side;
  if(SC_win_side == L_EYE)
    {
      SC_winner[1] = SC_l_maxx /** 16 +8*/; // * 16 to put it back in the original space! +8 to centre it in square
      SC_winner[2] = SC_l_maxy /** 16 +8*/;
    }
  else if(SC_win_side == R_EYE)
    {
      SC_winner[1] = SC_r_maxx /** 16 +8*/;
      SC_winner[2] = SC_r_maxy /** 16 +8*/;
    }
  
  return SC_winner;
}



//===================== ENVISION CLEANUP =================//
int envision_cleanup(void)
{
  struct env_alloc_stats stats;
  env_allocation_get_stats(&stats);
  env_stdio_print_alloc_stats(&stats, npixels ? npixels : 1);
  env_visual_cortex_destroy(&ivcL);
  env_visual_cortex_destroy(&ivcR);
  
  //REV: } close the unqualified { } section here... (important? for threading?)
  env_allocation_cleanup();
  
  return 0;
}//end envision cleanup


void SC_reset()
{
  RF = RF_baseline;
  
  for(int x=0; x<salmap_w; x++)
    {
      for(int y=0; y<salmap_h; y++)
	{
	  SC_l[x][y] = 0;
	  SC_r[x][y] = 0;
	  SC_win_side = -1;
	  SC_l_maxval = -1;
	  SC_r_maxval = -1;
	}
    }
}


float gaussian_val(float x_input, float sd, float mean, float scale)
{
  return scale*exp(-(((x_input-mean) * (x_input-mean))/(2*sd*sd)));
}

int x_loc(int array_loc, int width)
{
  return array_loc%width;
}

int y_loc(int array_loc, int width)
{
  return array_loc/width;
}

int x_y_to_array_loc(int x_pos, int y_pos, int width)
{
  return (width*(y_pos)) + x_pos;
}


// ######################################################################
// Thunk to convert from env_size_t to size_t
static void* malloc_thunk(env_size_t n)
{
  return malloc(n);
}
