#ifndef __services_user_H
#define __services_user_H

void LEDUpdate(void);
void LEDString(void);
void LED_show(uint16_t, uint16_t);
int MCP73831_state_detect(void);
void Power_off(void);
void Power_on(void);

#endif
