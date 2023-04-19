// Main.h file

// status info
typedef enum{
    AGV_OFF_STATE=0,
    AGV_INIT_DONE,
    AGV_IDLE_STATE,
    AGV_RETRACT_STATE,
    AGV_ERROR_STATE,
    AGV_STATUS_LAST
} AGV_STATUS;

typedef enum{
    AGV_REQ_NONE=0,
    AGV_REQ_MOVE,
    AGV_EMERGENCY_STOP,
    AGV_REQ_TEST
} AGV_REQ;



