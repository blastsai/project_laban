#ifndef SYSTEM_H
#define SYSTEM_H

extern volatile int system_state;
void init_system(void);      // Ham khoi tao he thong
void start_system(void);     // Ham bat dau he thong
void stop_system(void);      // Ham dung he thong
void control_LEDs(void);
#endif
