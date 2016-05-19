/** 3DGPL *************************************************\
 * ()                                                     *
 * 3-D transformations of coordinates.                    *
 *                                                        *
 * Ifdefs:                                                *
 *  _FLOAT_                  Use float implementation;    *
 *  _FIXED_                  Use fixed implementation;    *
 *  _Z_BUFFER_               Depth array;                 *
 *  _PAINTER_                Back to front order.         *
 *                                                        *
 * Defines:                                               *
 *  T_init_math              Creating sin/cos tables;     *
 *                                                        *
 *  T_translation            Translation of coordinates;  *
 *  T_scaling                Scaling coordinates;         *
 *                                                        *
 *  T_set_self_rotation      Setting object rotation;     *
 *  T_self_rotation          Rotation of coordinates;     *
 *  T_set_world_rotation     Setting viewer's rotation;   *
 *  T_world_rotation         Rotation of coordinates;     *
 *  T_cancatinate_self_world The two above matrices;      *
 *  T_cancatinated_rotation  From object into view space; *
 *                                                        *
 *  T_perspective            Transform to perspective;    *
 *                                                        *
 *  T_linear_solve           Gaussian elimination.        *
 *                                                        *
 * (c) 1995-98 Sergei Savchenko, (savs@cs.mcgill.ca)      *
\**********************************************************/

#include "RayTracing.h"           /* fast copy routines */
#include "Trans.h"                 /* 3D mathematics */
#include <math.h>                           /* sin & cos */

#define T_RADS 40.7436642                   /* pseudo-grads into rads */

int T_log_focus;                            /* foreshortening */

#if defined(_FIXED_)
typedef HW_32_bit T_fixed;                  /* better be 32bit machine */
T_fixed T_wx1,T_wx2,T_wx3,T_wy1,T_wy2,T_wy3,T_wz1,T_wz2,T_wz3;
T_fixed T_sx1,T_sx2,T_sx3,T_sy1,T_sy2,T_sy3,T_sz1,T_sz2,T_sz3;
T_fixed T_cx1,T_cx2,T_cx3,T_cy1,T_cy2,T_cy3,T_cz1,T_cz2,T_cz3;
T_fixed T_tx,T_ty,T_tz;
T_fixed T_sin[256],T_cos[256];              /* precalculated */
#endif
#if defined(_FLOAT_)
float T_wx1,T_wx2,T_wx3,T_wy1,T_wy2,T_wy3,T_wz1,T_wz2,T_wz3;
float T_sx1,T_sx2,T_sx3,T_sy1,T_sy2,T_sy3,T_sz1,T_sz2,T_sz3;
float T_cx1,T_cx2,T_cx3,T_cy1,T_cy2,T_cy3,T_cz1,T_cz2,T_cz3;
float T_tx,T_ty,T_tz;
float T_sin[256],T_cos[256];                /* precalculated */
#endif

/**********************************************************\
 * Initializing tables of trigonometric functions.        *
 *                                                        *
 * SETS: T_sin and T_cos arrays.                          *
 * -----                                                  *
\**********************************************************/

void T_init_math(void)
{
 int i;

 for(i=0;i<256;i++)
 {
#if defined(_FIXED_)
  T_sin[i]=sin(i/T_RADS)*(1<<T_P);          /* moving fraction into int part */
  T_cos[i]=cos(i/T_RADS)*(1<<T_P);          /* which is cast into fixed */
#endif
#if defined(_FLOAT_)
  T_sin[i]=sin(i/T_RADS);
  T_cos[i]=cos(i/T_RADS);
#endif
 }
}

/**********************************************************\
 * Translation of coordinates.                            *
\**********************************************************/


//register: the variable is to be stored in a machine register, if possible
void T_translation(int *from,register int *to,int length,
                   int addx,int addy,int addz
                  )
{
 register int i;

 for(i=0;i<length;i++)
 {
  *to++=(*from++)+addx;
  *to++=(*from++)+addy;
  *to++=(*from++)+addz;                     /* translating X Y Z */
  to++; from++;                             /* 4th element */
//the 4th element saves for what?
 }
}

/**********************************************************\
 * Scaling of coordinates.                                *
\**********************************************************/

void T_scaling(int *from,register int *to,int length,
               int mulx,int muly,int mulz
              )
{
 register int i;

 for(i=0;i<length;i++)
 {
  *to++=(*from++)*mulx;
  *to++=(*from++)*muly;
  *to++=(*from++)*mulz;                     /* scaling X Y Z */
  to++; from++;                             /* 4th element */
 }
}

/**********************************************************\
 * Constructing rotation matrix for object to world       *
 * rotation: (alp-bet-gam), roll then pitch then yaw      *
 * sequence. gam-yaw, bet-pitch, alp-roll. NB! angles     *
 * assumed to be in pseudo-degrees in the range [0,256).  *
 *                                                        *
 *          Y^    Z           x'=y*sin(alp)+x*cos(alp)    *
 *           |   /            y'=y*cos(alp)-x*sin(alp)    *
 *           |  / alp         z'=z                        *
 *          /|<---+                                       *
 *         | |/   |           x"=x'                       *
 *  -------|-+-----------> X  y"=y'*cos(bet)-z'*sin(bet)  *
 *     bet | |   __           z"=y'*sin(bet)+z'*cos(bet)  *
 *         V/|   /| gam                                   *
 *         /----+             x"'=z"*sin(gam)+x"*cos(gam) *
 *        /  |                y"'=y"                      *
 *       /   |                z"'=z"*cos(gam)-x"*sin(gam) *
 *           |                                            *
 *                                                        *
 * SETS: T_sx1,...,T_sz3.                                 *
 * -----                                                  *
\**********************************************************/

void T_set_self_rotation(unsigned char alp,unsigned char bet,
                         unsigned char gam)
{
#if defined(_FIXED_)
 T_fixed cosalp,sinalp,cosbet,sinbet,cosgam,singam;
#endif
#if defined(_FLOAT_)
 float cosalp,sinalp,cosbet,sinbet,cosgam,singam;
#endif

 cosalp=T_cos[alp];
 sinalp=T_sin[alp];
 cosbet=T_cos[bet];
 sinbet=T_sin[bet];
 cosgam=T_cos[gam];
 singam=T_sin[gam];                         /* initializing */

#if defined(_FIXED_)
 T_sx1=((cosalp*cosgam)>>T_P)-((sinalp*((sinbet*singam)>>T_P))>>T_P);
 T_sy1=((sinalp*cosgam)>>T_P)+((cosalp*((sinbet*singam)>>T_P))>>T_P);
 T_sz1=((cosbet*singam)>>T_P);
 T_sx2=-((sinalp*cosbet)>>T_P);
 T_sy2=((cosalp*cosbet)>>T_P);
 T_sz2=-sinbet;
 T_sx3=-((cosalp*singam)>>T_P)-((sinalp*((sinbet*cosgam)>>T_P))>>T_P);
 T_sy3=((cosalp*((sinbet*cosgam)>>T_P))>>T_P)-((sinalp*singam)>>T_P);
 T_sz3=((cosbet*cosgam)>>T_P);              /* calculating the matrix */
#endif
#if defined(_FLOAT_)
 T_sx1=cosalp*cosgam-sinalp*sinbet*singam;
 T_sy1=sinalp*cosgam+cosalp*sinbet*singam;
 T_sz1=cosbet*singam;
 T_sx2=-sinalp*cosbet;
 T_sy2=cosalp*cosbet;
 T_sz2=-sinbet;
 T_sx3=-cosalp*singam-sinalp*sinbet*cosgam;
 T_sy3=cosalp*sinbet*cosgam-sinalp*singam;
 T_sz3=cosbet*cosgam;                       /* calculating the matrix */
#endif
}

/**********************************************************\
 * Rotating the coordinates object to world.              *
 *                                                        *
 *                                       |sx1 sx2 sx3|    *
 * T'=T[S]  where:  [x' y' z'] = [x y z]*|sy1 sy2 sy3|    *
 *                                       |sz1 sz2 sz3|    *
 *                                                        *
\**********************************************************/

void T_self_rotation(int *from,register int *to,int length)
{
 register int i,xt,yt,zt;

 for(i=0;i<length;i++)
 {
  xt=*from++; yt=*from++; zt=*from++;

#if defined(_FIXED_)
  *to++=(int)(((T_sx1*xt)>>T_P) + ((T_sy1*yt)>>T_P) + ((T_sz1*zt)>>T_P));
  *to++=(int)(((T_sx2*xt)>>T_P) + ((T_sy2*yt)>>T_P) + ((T_sz2*zt)>>T_P));
  *to++=(int)(((T_sx3*xt)>>T_P) + ((T_sy3*yt)>>T_P) + ((T_sz3*zt)>>T_P));
#endif
#if defined(_FLOAT_)
  *to++=(int)(T_sx1*xt + T_sy1*yt + T_sz1*zt);
  *to++=(int)(T_sx2*xt + T_sy2*yt + T_sz2*zt);
  *to++=(int)(T_sx3*xt + T_sy3*yt + T_sz3*zt);
#endif
  to++; from++;                             /* 4th element */
 }
}

/**********************************************************\
 * Constructing rotation matrix world to view:            *
 * (gam-bet-alp), yaw, then pitch then roll sequence.     *
 * gam-yaw, bet-pitch alp-roll. NB! angles assumed to be  *
 * in pseudo-degrees in the range [0,256).                *
 *                                                        *
 *          Y^    Z           x'=z*sin(gam)+x*cos(gam)    *
 *           |   /            y'=y                        *
 *           |  / alp         z'=z*cos(gam)-x*sin(gam)    *
 *          /|<---+                                       *
 *         | |/   |           x"=x'                       *
 *  -------|-+-----------> X  y"=y'*cos(bet)-z'*sin(bet)  *
 *     bet | |   __           z"=y'*sin(bet)+z'*cos(bet)  *
 *         V/|   /| gam                                   *
 *         /----+             x"'=y"*sin(alp)+x"*cos(alp) *
 *        /  |                y"'=y"*cos(alp)-x"*sin(alp) *
 *       /   |                z"'=z"                      *
 *           |                                            *
 *                                                        *
 * SETS: T_wx1,...,T_wz3.                                 *
 * -----                                                  *
\**********************************************************/

void T_set_world_rotation(unsigned char alp,unsigned char bet,
                          unsigned char gam)
{
#if defined(_FIXED_)
 T_fixed cosalp,sinalp,cosbet,sinbet,cosgam,singam;
#endif
#if defined(_FLOAT_)
 float cosalp,sinalp,cosbet,sinbet,cosgam,singam;
#endif

 cosalp=T_cos[alp];
 sinalp=T_sin[alp];
 cosbet=T_cos[bet];
 sinbet=T_sin[bet];
 cosgam=T_cos[gam];
 singam=T_sin[gam];                         /* initializing */

#if defined(_FIXED_)
 T_wx1=((singam*((sinbet*sinalp)>>T_P))>>T_P) + ((cosgam*cosalp)>>T_P);
 T_wy1=((cosbet*sinalp)>>T_P);
 T_wz1=((singam*cosalp)>>T_P) - ((cosgam*((sinbet*sinalp)>>T_P))>>T_P);
 T_wx2=((singam*((sinbet*cosalp)>>T_P))>>T_P) - ((cosgam*sinalp)>>T_P);
 T_wy2=((cosbet*cosalp)>>T_P);
 T_wz2=-((cosgam*((sinbet*cosalp)>>T_P))>>T_P) - ((singam*sinalp)>>T_P);
 T_wx3=-((singam*cosbet)>>T_P);
 T_wy3=sinbet;
 T_wz3=((cosgam*cosbet)>>T_P);              /* calculating the matrix */
#endif
#if defined(_FLOAT_)
 T_wx1=singam*sinbet*sinalp + cosgam*cosalp;
 T_wy1=cosbet*sinalp;
 T_wz1=singam*cosalp - cosgam*sinbet*sinalp;
 T_wx2=singam*sinbet*cosalp - cosgam*sinalp;
 T_wy2=cosbet*cosalp;
 T_wz2=-cosgam*sinbet*cosalp - singam*sinalp;
 T_wx3=-singam*cosbet;
 T_wy3=sinbet;
 T_wz3=cosgam*cosbet;                       /* calculating the matrix */
#endif
}

/**********************************************************\
 * Rotating the coordinates world to view.                *
 *                                                        *
 *                                       |wx1 wx2 wx3|    *
 * T'=T[W]  where:  [x' y' z'] = [x y z]*|wy1 wy2 wy3|    *
 *                                       |wz1 wz2 wz3|    *
 *                                                        *
\**********************************************************/

void T_world_rotation(int *from,register int *to,int length)
{
 register int i,xt,yt,zt;

 for(i=0;i<length;i++)
 {
  xt=*from++; yt=*from++; zt=*from++;

#if defined(_FIXED_)
  *to++=(int)(((T_wx1*xt)>>T_P) + ((T_wy1*yt)>>T_P) + ((T_wz1*zt)>>T_P));
  *to++=(int)(((T_wx2*xt)>>T_P) + ((T_wy2*yt)>>T_P) + ((T_wz2*zt)>>T_P));
  *to++=(int)(((T_wx3*xt)>>T_P) + ((T_wy3*yt)>>T_P) + ((T_wz3*zt)>>T_P));
#endif
#if defined(_FLOAT_)
  *to++=(int)(T_wx1*xt+T_wy1*yt+T_wz1*zt);
  *to++=(int)(T_wx2*xt+T_wy2*yt+T_wz2*zt);
  *to++=(int)(T_wx3*xt+T_wy3*yt+T_wz3*zt);
#endif
  to++; from++;                             /* 4th element */
 }
}

/**********************************************************\
 * Cancatinating matrices of object to world rotation,    *
 * some translation (may have been object and camera      *
 * translations combined earlier) and world to view       *
 * rotation to obtain a single transformation from object *
 * to view space.                                         *
 *                                                        *
 *                     [K]=[S][T][W]                      *
 *                                                        *
 *                                                        *
 * SETS: T_kx1,...,T_kz3,T_tx,T_ty,T_tz.                  *
 * -----                                                  *
\**********************************************************/

void T_cancatinate_self_world(int addx,int addy,int addz)
{
#if defined(_FIXED_)
 T_cx1=(int)(((T_sx1*T_wx1)>>T_P)+((T_sx2*T_wy1)>>T_P)+((T_sx3*T_wz1)>>T_P));
 T_cx2=(int)(((T_sx1*T_wx2)>>T_P)+((T_sx2*T_wy2)>>T_P)+((T_sx3*T_wz2)>>T_P));
 T_cx3=(int)(((T_sx1*T_wx3)>>T_P)+((T_sx2*T_wy3)>>T_P)+((T_sx3*T_wz3)>>T_P));

 T_cy1=(int)(((T_sy1*T_wx1)>>T_P)+((T_sy2*T_wy1)>>T_P)+((T_sy3*T_wz1)>>T_P));
 T_cy2=(int)(((T_sy1*T_wx2)>>T_P)+((T_sy2*T_wy2)>>T_P)+((T_sy3*T_wz2)>>T_P));
 T_cy3=(int)(((T_sy1*T_wx3)>>T_P)+((T_sy2*T_wy3)>>T_P)+((T_sy3*T_wz3)>>T_P));

 T_cz1=(int)(((T_sz1*T_wx1)>>T_P)+((T_sz2*T_wy1)>>T_P)+((T_sz3*T_wz1)>>T_P));
 T_cz2=(int)(((T_sz1*T_wx2)>>T_P)+((T_sz2*T_wy2)>>T_P)+((T_sz3*T_wz2)>>T_P));
 T_cz3=(int)(((T_sz1*T_wx3)>>T_P)+((T_sz2*T_wy3)>>T_P)+((T_sz3*T_wz3)>>T_P));

 T_tx=(int)(((addx*T_wx1)>>T_P)+((addy*T_wy1)>>T_P)+((addz*T_wz1)>>T_P));
 T_ty=(int)(((addx*T_wx2)>>T_P)+((addy*T_wy2)>>T_P)+((addz*T_wz2)>>T_P));
 T_tz=(int)(((addx*T_wx3)>>T_P)+((addy*T_wy3)>>T_P)+((addz*T_wz3)>>T_P));
#endif
#if defined(_FLOAT_)
 T_cx1=T_sx1*T_wx1+T_sx2*T_wy1+T_sx3*T_wz1;
 T_cx2=T_sx1*T_wx2+T_sx2*T_wy2+T_sx3*T_wz2;
 T_cx3=T_sx1*T_wx3+T_sx2*T_wy3+T_sx3*T_wz3;

 T_cy1=T_sy1*T_wx1+T_sy2*T_wy1+T_sy3*T_wz1;
 T_cy2=T_sy1*T_wx2+T_sy2*T_wy2+T_sy3*T_wz2;
 T_cy3=T_sy1*T_wx3+T_sy2*T_wy3+T_sy3*T_wz3;

 T_cz1=T_sz1*T_wx1+T_sz2*T_wy1+T_sz3*T_wz1;
 T_cz2=T_sz1*T_wx2+T_sz2*T_wy2+T_sz3*T_wz2;
 T_cz3=T_sz1*T_wx3+T_sz2*T_wy3+T_sz3*T_wz3;

 T_tx=addx*T_wx1+addy*T_wy1+addz*T_wz1;
 T_ty=addx*T_wx2+addy*T_wy2+addz*T_wz2;
 T_tz=addx*T_wx3+addy*T_wy3+addz*T_wz3;
#endif
}

/**********************************************************\
 * Transforming the coordinates object to view.           *
 *                                                        *
 *                                       |cx1 cx2 cx3 0|  *
 * T'=T[K] where: [x' y' z' 1]=[x y z 1]*|cy1 cy2 cy3 0|  *
 *                                       |cz1 cz2 cz3 0|  *
 *                                       |tx  ty  tz  1|  *
 *                                                        *
\**********************************************************/

void T_cancatinated_rotation(int *from,register int *to,int length)
{
 register int i,xt,yt,zt;

 for(i=0;i<length;i++)
 {
  xt=*from++; yt=*from++; zt=*from++;

#if defined(_FIXED_)
  *to++=(int)(((T_cx1*xt)>>T_P)+((T_cy1*yt)>>T_P)+((T_cz1*zt)>>T_P)+T_tx);
  *to++=(int)(((T_cx2*xt)>>T_P)+((T_cy2*yt)>>T_P)+((T_cz2*zt)>>T_P)+T_ty);
  *to++=(int)(((T_cx3*xt)>>T_P)+((T_cy3*yt)>>T_P)+((T_cz3*zt)>>T_P)+T_tz);
#endif
#if defined(_FLOAT_)
  *to++=(int)(T_cx1*xt+T_cy1*yt+T_cz1*zt+T_tx);
  *to++=(int)(T_cx2*xt+T_cy2*yt+T_cz2*zt+T_ty);
  *to++=(int)(T_cx3*xt+T_cy3*yt+T_cz3*zt+T_tz);
#endif
  to++; from++;                             /* 4th element */
 }
}

/**********************************************************\
 * Transforming to perspective, the coordinates passed    *
 * are supposed to be both volume, and Z-clipped,         *
 * otherwise division by 0 or overflow may occur.         *
 * Also, performs the screen centre translation.          *
 *                                                        *
 *                *                                       *
 *               /|X                                      *
 *              / |                                       *
 *             /  |                                       *
 *            *   |                                       *
 *           /|X' |       X'      X                       *
 *          / |   |    ------- = ---                      *
 *         /  |   |     focus     Z                       *
 *        *---+---+                                       *
 *        0   ^   Z    X'= X*focus/Z                      *
 *            |                                           *
 *          focus                                         *
 *                                                        *
\**********************************************************/

void T_perspective(int *from,register int *to,int dimension,
                   int length,int log_focus
                  )
{
 register int i;
 int rem_dim=dimension-3;                   /* other then X Y Z */

 for(i=0;i<length;i++,from+=rem_dim,to+=rem_dim)
 {                                          /* NB! Z is not changed */
  *to++=(int)((((long)from[0])<<log_focus)/from[2])+HW_SCREEN_X_CENTRE;
  *to++=(int)((((long)from[1])<<log_focus)/from[2])+HW_SCREEN_Y_CENTRE;
                                            /* also translates to screen */
#if defined(_Z_BUFFER_)                     /* centre */
  *to++=T_ONE_OVER_Z_CONST/(long)from[2];   /* 1/Z is left in the tuple */
#endif

  HW_copy_int(from+=3,to,rem_dim);          /* remaining values are passed */
 }
}

/**********************************************************\
 * Gaussian elimination.                                  *
\**********************************************************/

void T_linear_solve(int ia[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE],
                    int ib[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE],
                    int ix[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE],
                    int n,int m,
                    int log_source_multiplyer,
                    int log_result_multiplyer
                   )
{
#if defined(_FIXED_)
 T_fixed a[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE];
 T_fixed b[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE];
 T_fixed x[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE];
 T_fixed max,tmp,pivot,sum;
#endif
#if defined(_FLOAT_)
 float a[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE];
 float b[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE];
 float x[T_MAX_MATRIX_SIZE][T_MAX_MATRIX_SIZE];
 float max,tmp,pivot,sum;
#endif

 int i,j,k,num;

 for(i=0;i<n;i++)                           /* into internal representation */
 {
#if defined(_FIXED_)
  for(j=0;j<n;j++) a[i][j]=(T_fixed)(ia[i][j]>>log_source_multiplyer);
  for(j=0;j<m;j++) b[i][j]=(T_fixed)ib[i][j];
#endif
#if defined(_FLOAT_)
  for(j=0;j<n;j++) a[i][j]=(float)(ia[i][j]>>log_source_multiplyer);
  for(j=0;j<m;j++) b[i][j]=(float)ib[i][j];
#endif
 }

 for(max=0,num=0,i=0;i<n-1;i++)
 {
  for(j=i;j<n;j++)                          /* looking for pivot element */
  {
   if(a[j][i]>=0) pivot=a[j][i]; else pivot=-a[j][i];
   if(pivot>max) { max=pivot; num=j; }
  }

  if(max!=0)                                /* for non zero pivots */
  {
   if(num!=i)                               /* swap is requied */
   {
    for(j=0;j<n;j++) { tmp=a[i][j]; a[i][j]=a[num][j]; a[num][j]=tmp; }
    for(j=0;j<m;j++) { tmp=b[i][j]; b[i][j]=b[num][j]; b[num][j]=tmp; }
   }

   for(j=i+1;j<n;j++)                       /* eliminating all coefs below */
   {
    for(k=i+1;k<n;k++) a[j][k]=a[j][k]-a[i][k]*a[j][i]/a[i][i];
    for(k=0;k<m;k++)   b[j][k]=b[j][k]-b[i][k]*a[j][i]/a[i][i];
   }
  }
 }

 for(i=n-1;i>=0;i--)                        /* reversed direction */
 {
  for(k=0;k<m;k++)
  {
#if defined(_FIXED_)
   for(sum=0,j=i+1;j<n;j++) sum=sum+((a[i][j]*x[j][k])>>log_result_multiplyer);
   x[i][k]=(((b[i][k])-sum)<<log_result_multiplyer)/a[i][i];
   ix[i][k]=(int)x[i][k];                   /* external form for the result */
#endif
#if defined(_FLOAT_)
   for(sum=0,j=i+1;j<n;j++) sum=sum+a[i][j]*x[j][k];
   x[i][k]=(b[i][k]-sum)/a[i][i];           /* external form for the result */
   ix[i][k]=(int)(x[i][k]*(0x1<<log_result_multiplyer));
#endif
  }
 }
}

/**********************************************************/
