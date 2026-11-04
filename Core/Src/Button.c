#include "button.h"
#include "main.h"

#define DEBOUNCE_MS 50u
#define POLL_MS 5u
#define LONG_PRESS_MS 1000u

volatile uint32_t interruptCount = 0;
static volatile uint32_t lastInterruptMs = 0;
static uint32_t handledCount = 0;

void onButtonInterrupt(void)
{
	interruptCount++;
	lastInterruptMs = HAL_GetTick();
}

static bool isButtonDown(void)
{
	return HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET;
}

static bool hasNewInterrupt(void)
{
	return handledCount != interruptCount;
}

static bool isContactQuiet(uint32_t now)
{
	return now - lastInterruptMs >= DEBOUNCE_MS;
}

// Task 1

static bool noDebounce(void)
{
	if (!hasNewInterrupt()) {
		return false;
	}

	handledCount++;
	return true;
}

// Task 2

static bool timeDebounce(void)
{
	static uint32_t lastAcceptedMs = 0;

	if (!hasNewInterrupt()) {
		return false;
	}
	handledCount = interruptCount;

	if (lastInterruptMs - lastAcceptedMs < DEBOUNCE_MS) {
		return false;
	}

	lastAcceptedMs = lastInterruptMs;
	return true;
}

// Task 3

static bool stateDebounce(uint32_t now)
{
	if (!hasNewInterrupt() || !isContactQuiet(now)) {
		return false;
	}
	handledCount = interruptCount;

	return isButtonDown();
}

// Task 4

typedef enum {
	RELEASED,
	MAYBE_PRESSED,
	PRESSED,
	MAYBE_RELEASED
} PollState;

static bool pollingDebounce(uint32_t now)
{
	static uint32_t lastPollMs = 0;
	static PollState state = RELEASED;
	static uint32_t changedMs = 0;

	if (now - lastPollMs < POLL_MS) {
		return false;
	}
	lastPollMs = now;

	const bool down = isButtonDown();
	const bool stableLongEnough = now - changedMs >= DEBOUNCE_MS;

	switch (state) {
		case RELEASED:
			if (down) {
				state = MAYBE_PRESSED;
				changedMs = now;
			}
			return false;

		case MAYBE_PRESSED:
			if (!down) {
				state = RELEASED;
				return false;
			}
			if (stableLongEnough) {
				state = PRESSED;
				return true;
			}
			return false;

		case PRESSED:
			if (!down) {
				state = MAYBE_RELEASED;
				changedMs = now;
			}
			return false;

		case MAYBE_RELEASED:
			if (down) {
				state = PRESSED;
				return false;
			}
			if (stableLongEnough) {
				state = RELEASED;
			}
			return false;
	}
	return false;
}

// Extra homework

typedef enum {
	NO_CHANGE,
	JUST_PRESSED,
	JUST_RELEASED
} ButtonChange;

static bool buttonDown = false;
static uint32_t pressedMs = 0;

static ButtonChange readButtonChange(uint32_t now)
{
	if (!hasNewInterrupt() || !isContactQuiet(now)) {
		return NO_CHANGE;
	}
	handledCount = interruptCount;

	const bool down = isButtonDown();
	if (down == buttonDown) {
		return NO_CHANGE;
	}

	buttonDown = down;
	if (down) {
		pressedMs = lastInterruptMs;
		return JUST_PRESSED;
	}
	return JUST_RELEASED;
}

static bool shortPress(uint32_t now)
{
	return readButtonChange(now) == JUST_RELEASED && now - pressedMs < LONG_PRESS_MS;
}

static bool longPress(uint32_t now)
{
	static bool alreadyFired = false;

	if (readButtonChange(now) == JUST_PRESSED) {
		alreadyFired = false;
	}

	if (buttonDown && !alreadyFired && now - pressedMs >= LONG_PRESS_MS) {
		alreadyFired = true;
		return true;
	}
	return false;
}

static bool afterRelease(uint32_t now)
{
	return readButtonChange(now) == JUST_RELEASED;
}

bool buttonPressed(uint32_t now)
{
	switch (MODE) {
		case MODE_NO_DEBOUNCE:    return noDebounce();
		case MODE_TIME_DEBOUNCE:  return timeDebounce();
		case MODE_STATE_DEBOUNCE: return stateDebounce(now);
		case MODE_POLLING:        return pollingDebounce(now);
		case MODE_SHORT_PRESS:    return shortPress(now);
		case MODE_LONG_PRESS:     return longPress(now);
		case MODE_ON_RELEASE:     return afterRelease(now);
	}
	return false;
}
