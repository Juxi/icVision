// approx time between stream packets from robot
#define YARP_PERIOD 0.02					// seconds
#define YARP_PERIOD_ms YARP_PERIOD*1000		// milliseconds
#define YARP_PERIOD_us YARP_PERIOD_ms*1000	// microseconds

// for reflex behavior
#define POSE_BUFFER_SIZE 100
#define REFLEX_SPEED 30
#define POSITION_MOVE_TIMEOUT 10 //seconds
#define NEGLIGIBLE_ANGLE 5 //degrees