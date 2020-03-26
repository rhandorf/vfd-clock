/*
 * Configuration options for the VFD Clock
 */

/* SSID to connect to (must support 2.4GHz channels and WPA-Personal) */
const char *ssid     = "SSID";
/* WPA/WPA2 password/PSK */
const char *password = "PASSWORD";

/* Time offset from UTC */
const long utcOffsetInSeconds = 3600*-5;

/* Comment out to disable casino mode each minute */
#define SPIN_CASINO

/* Change the offset in colors between hours, minutes, seconds */
#define COLOR_OFFSET    60
