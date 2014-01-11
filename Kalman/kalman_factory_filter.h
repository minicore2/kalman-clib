/**
/* \brief Initializes a named Kalman filter structure.
*
* This include requires the three defines {\ref KALMAN_NAME}, {\ref KALMAN_NUM_STATES} and
* {\ref KALMAN_NUM_INPUTS} to be set to the base name of the Kalman Filter and to the number 
* of states and inputs respectively.
*
* It then will instantiate the buffers required for A, P, x as well as B, Q and u if the number
* of inputs is greater than zero, as well as the structure for the Kalman filter and the
* initialization method.
*
* Suppose the Kalman filter shall be named "acceleration", has three states and zero inputs.
* You would start by defining the required macros
*
* \code{.c}
* #define KALMAN_NAME acceleration
* #define KALMAN_NUM_STATES 3
* #define KALMAN_NUM_INPUTS 0
* \endcode
*
* After that, this file must be included
*
* \code{.c}
* #include "kalman_factory_filter_init.h"
* \endcode
*
* At this point, the structure \c kalman_filter_acceleration will be created (statically) along with
* all the required buffers (i.e. \c kalman_filter_acceleration_A_buffer, etc.) and the matrices
* will be initialized and set with the correct dimensions.
*
* In addition, a parameterless static initialization function \code {kalman_filter_acceleration_init()} will
* be created which you will need to call manually in order to set up the filter.
*
* To clean up the defined macros (e.g. in order to be able to create another named Kalman filter),
* you will have to include kalman_factory_cleanup.h:
*
* \code{.c}
* #include "kalman_factory_cleanup.h"
* \endcode
*
* A full example would be

* \code{.c}
* #define KALMAN_NAME example
* #define KALMAN_NUM_STATES 4
* #define KALMAN_NUM_INPUTS 0

* #include "kalman_factory_filter.h"
* // NOTE that this is the point to create measurement buffers
* #include "kalman_factory_cleanup.h"

* void test_kalman()
* {
*   kalman_filter_example_init();
*   kalman_filter_example.x.data[0] = 1;
* }
* \endcode
*/

/************************************************************************/
/* Check for inputs                                                     */
/************************************************************************/

#ifndef KALMAN_NAME
#error KALMAN_NAME needs to be defined prior to inclusion of this file.
#endif

#ifndef KALMAN_NUM_STATES
#error KALMAN_NUM_STATES needs to be defined prior to inclusion of this file.
#elif KALMAN_NUM_STATES <= 0
#error KALMAN_NUM_STATES must be a positive integer
#endif

#ifndef KALMAN_NUM_INPUTS
#error KALMAN_NUM_INPUTS needs to be defined prior to inclusion of this file.
#elif KALMAN_NUM_INPUTS < 0
#error KALMAN_NUM_INPUTS must be a positive integer or zero if no inputs are used
#endif

/************************************************************************/
/* Prepare dimensions                                                   */
/************************************************************************/

#define __KALMAN_A_ROWS     KALMAN_NUM_STATES
#define __KALMAN_A_COLS     KALMAN_NUM_STATES

#define __KALMAN_P_ROWS     KALMAN_NUM_STATES
#define __KALMAN_P_COLS     KALMAN_NUM_STATES

#define __KALMAN_x_ROWS     KALMAN_NUM_STATES
#define __KALMAN_x_COLS     1

#define __KALMAN_B_ROWS     KALMAN_NUM_STATES
#define __KALMAN_B_COLS     KALMAN_NUM_INPUTS

#define __KALMAN_u_ROWS     KALMAN_NUM_INPUTS
#define __KALMAN_u_COLS     1

#define __KALMAN_Q_ROWS     KALMAN_NUM_INPUTS
#define __KALMAN_Q_COLS     KALMAN_NUM_INPUTS

/************************************************************************/
/* Name helper macro                                                    */
/************************************************************************/

#ifndef STRINGIFY
#define __STRING2(x) #x
#define STRINGIFY(x) __STRING2(x)
#endif

#pragma message("** Instantiating Kalman filter \"" STRINGIFY(KALMAN_NAME) "\" with " STRINGIFY(KALMAN_NUM_STATES) " states and " STRINGIFY(KALMAN_NUM_INPUTS) " inputs")

#define __CONCAT(x, y)                                  x ## y

#define KALMAN_FILTER_BASENAME_HELPER(name)             __CONCAT(kalman_filter_, name)
#define KALMAN_FILTER_BASENAME                          KALMAN_FILTER_BASENAME_HELPER(KALMAN_NAME)
#define KALMAN_BASENAME_HELPER(basename)                __CONCAT(basename, _)

/************************************************************************/
/* Name macro                                                           */
/************************************************************************/

#define KALMAN_BUFFER_NAME(element)                     KALMAN_BASENAME_HELPER(KALMAN_FILTER_BASENAME) ## element ## _buffer
#define KALMAN_FUNCTION_NAME(name)                      KALMAN_BASENAME_HELPER(KALMAN_FILTER_BASENAME) ## name
#define KALMAN_STRUCT_NAME                              KALMAN_FILTER_BASENAME

/************************************************************************/
/* Construct Kalman filter                                              */
/************************************************************************/

#include "compiler.h"
#include "matrix.h"
#include "kalman.h"

#define __KALMAN_BUFFER_A   KALMAN_BUFFER_NAME(A)
#define __KALMAN_BUFFER_P   KALMAN_BUFFER_NAME(P)
#define __KALMAN_BUFFER_x   KALMAN_BUFFER_NAME(x)

#pragma message("Creating Kalman filter A buffer: " STRINGIFY(__KALMAN_BUFFER_A))
static matrix_data_t __KALMAN_BUFFER_A[__KALMAN_A_ROWS * __KALMAN_A_COLS];

#pragma message("Creating Kalman filter P buffer: " STRINGIFY(__KALMAN_BUFFER_P))
static matrix_data_t __KALMAN_BUFFER_P[__KALMAN_P_ROWS * __KALMAN_P_COLS];

#pragma message("Creating Kalman filter x buffer: " STRINGIFY(__KALMAN_BUFFER_x))
static matrix_data_t __KALMAN_BUFFER_x[__KALMAN_x_ROWS * __KALMAN_x_COLS];

#if KALMAN_NUM_INPUTS > 0

#define __KALMAN_BUFFER_B   KALMAN_BUFFER_NAME(B)
#define __KALMAN_BUFFER_Q   KALMAN_BUFFER_NAME(Q)
#define __KALMAN_BUFFER_u   KALMAN_BUFFER_NAME(u)

#pragma message("Creating Kalman filter B buffer: " STRINGIFY(__KALMAN_BUFFER_B))
static matrix_data_t __KALMAN_BUFFER_B[__KALMAN_B_ROWS * __KALMAN_B_COLS];

#pragma message("Creating Kalman filter Q buffer: " STRINGIFY(__KALMAN_BUFFER_Q))
static matrix_data_t __KALMAN_BUFFER_Q[__KALMAN_Q_ROWS * __KALMAN_Q_COLS];

#pragma message("Creating Kalman filter u buffer: " STRINGIFY(__KALMAN_BUFFER_u))
static matrix_data_t __KALMAN_BUFFER_u[__KALMAN_x_ROWS * __KALMAN_u_COLS];

#else

#pragma message("Creating Kalman filter B buffer: skipped (zero inputs)")
#define __KALMAN_BUFFER_B ((matrix_data_t*)0)

#pragma message("Creating Kalman filter Q buffer: skipped (zero inputs)")
#define __KALMAN_BUFFER_Q ((matrix_data_t*)0)

#pragma message("Creating Kalman filter u buffer: skipped (zero inputs)")
#define __KALMAN_BUFFER_u ((matrix_data_t*)0)

#endif

#pragma message("Creating Kalman filter structure: " STRINGIFY(KALMAN_STRUCT_NAME))

/*!
* \brief The Kalman filter structure
*/
static kalman_t KALMAN_STRUCT_NAME;

#pragma message ("Creating Kalman filter initialization function: " STRINGIFY(KALMAN_FUNCTION_NAME(init()) ))

/*!
* \brief Initializes the Kalman Filter
*/
STATIC_INLINE KALMAN_FUNCTION_NAME(init)()
{
    kalman_filter_initialize(&KALMAN_STRUCT_NAME, KALMAN_NUM_STATES, KALMAN_NUM_INPUTS, __KALMAN_BUFFER_A, __KALMAN_BUFFER_x, __KALMAN_BUFFER_B, __KALMAN_BUFFER_u, __KALMAN_BUFFER_P, __KALMAN_BUFFER_Q);
}
