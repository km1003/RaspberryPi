#define FALSE	(0)
#define TRUE	(1)

// RPi board revision 2
#define REV2	TRUE

// Broadcom GPIO map
#define SHIFTEN	(4)
#define TXD	(14)
#define RXD	(15)
#define M1POL	(17)
#define PWM	(18)
#if REV2
#define M2POL	(27)
#else
#define M2POL	(21)
#endif
#define PWMRNG	(22)
#define	COMPTRG	(23)

typedef char	bool;


// Function prototypes
void init(void);
void turnLeft(int speed);
void turnRight(int speed);
void driveForward(int speed);
void driveReverse(int speed);
void stop(void);

