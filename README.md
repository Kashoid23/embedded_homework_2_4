## Обладнання

- **STM32F411CEU6**
- кнопка
- 2 світлодіоди і 2 резистори 330 Ом

## Схема підключення

<img width="2720" height="1200" alt="stm32f411_button_debounce_schematic_clean" src="https://github.com/user-attachments/assets/3de212de-9d03-42cf-9150-d452bfd766c6" />

## Налаштування CubeMX

<img width="1863" height="805" alt="Знімок екрана 2026-11-04 181103" src="https://github.com/user-attachments/assets/8962ef3b-a4a3-466d-8c68-539ebed73c3c" />
<img width="1863" height="802" alt="Знімок екрана 2026-11-04 181232" src="https://github.com/user-attachments/assets/1a39fe1d-04d6-4d2a-9a1e-de818ed76d8a" />
<img width="1864" height="805" alt="Знімок екрана 2026-11-04 181141" src="https://github.com/user-attachments/assets/e1d17562-6e3c-43e0-bdfc-9610a092a0af" />
<img width="1863" height="797" alt="Знімок екрана 2026-11-04 181307" src="https://github.com/user-attachments/assets/cf0a6990-f367-4231-ba75-219057328593" />
<img width="1861" height="814" alt="Знімок екрана 2026-11-04 181410" src="https://github.com/user-attachments/assets/ecd59164-0cc4-46de-8218-53d59bd60db1" />

## Режими

Режим вибирається в `Core/Inc/button.h`:

```c
#define MODE MODE_STATE_DEBOUNCE
```

| `MODE` | Завдання | Коли спрацьовує дія |
|---|---|---|
| 1 `MODE_NO_DEBOUNCE` | 1. Без debounce | на кожне переривання |
| 2 `MODE_TIME_DEBOUNCE` | 2. Time-based | переривання не частіше ніж раз на 50 мс |
| 3 `MODE_STATE_DEBOUNCE` | 3. State-based | якщо через 50 мс після переривання кнопка досі натиснута |
| 4 `MODE_POLLING` | 4. Polling + автомат станів | пін опитується кожні 5 мс, натискання підтверджується через 50 мс |
| 5 `MODE_SHORT_PRESS` | Дод. 1. Коротке натискання | після відпускання, якщо кнопку тримали менше 1 с |
| 6 `MODE_LONG_PRESS` | Дод. 2. Довге натискання | коли кнопку тримають 1 с |
| 7 `MODE_ON_RELEASE` | Дод. 3. Після відпускання | у момент відпускання |

Після зміни режиму проєкт треба перезібрати і завантажити на плату.

## Як це працює

1. **Переривання** (`onButtonInterrupt`) лише рахує спрацювання і запам'ятовує час останнього. Уся логіка виконується в циклі.
2. **Цикл** викликає `buttonPressed(now)`. Функція повертає `true`, коли треба виконати дію, і тоді перемикається LED1.
3. **Heartbeat.** LED2 блимає кожні 500 мс. Це показує, що цикл не блокується.

## Структура коду

| Файл | Вміст |
|---|---|
| `Core/Inc/button.h` | константи режимів, `MODE`, оголошення функцій |
| `Core/Src/button.c` | логіка всіх режимів |
| `Core/Src/main.c` | дія на натискання, heartbeat, callback переривання (блоки `USER CODE`) |

## Результати

Лічильники дивляться в STM32CubeIDE: **Live Expressions**.

| Змінна | Що показує |
|---|---|
| `interruptCount` | усі переривання, разом із брязкотом |
| `pressCount` | скільки разів виконалась дія |

### Порівняльна таблиця (10 натискань)

| Режим | Без RC: `interruptCount` / `pressCount` | Реакція на відпускання | Затримка реакції |
|---|---|---|---|
| 1. Без debounce | <img width="588" height="59" alt="Знімок екрана 2026-11-04 184534" src="https://github.com/user-attachments/assets/21b2563e-e208-401c-96d0-d016c92b0120" /> | так | немає |
| 2. Time-based | <img width="586" height="58" alt="Знімок екрана 2026-11-04 184835" src="https://github.com/user-attachments/assets/1088f774-30ef-4e62-9125-5a0939f98314" /> | можлива | немає |
| 3. State-based | <img width="586" height="59" alt="Знімок екрана 2026-11-04 185200" src="https://github.com/user-attachments/assets/69ff2d3d-ce96-49df-85fe-c76f37348ffd" /> | ні | ~50 мс |
| 4. Polling | <img width="589" height="61" alt="Знімок екрана 2026-11-04 185344" src="https://github.com/user-attachments/assets/e7b69adc-b3df-4941-8a30-540db0012e1d" /> | ні | ~50 мс |
| 5. Short press | <img width="587" height="58" alt="Знімок екрана 2026-11-04 185516" src="https://github.com/user-attachments/assets/9f782eb2-1703-4b02-96ab-79c0d104316b" /> | — | після відпускання |
| 6. Long press | <img width="587" height="58" alt="Знімок екрана 2026-11-04 185700" src="https://github.com/user-attachments/assets/bb54fb36-c474-49b2-b93b-fd101ee11bd3" /> | — | 1 с |
| 7. On release | <img width="586" height="59" alt="Знімок екрана 2026-11-04 185910" src="https://github.com/user-attachments/assets/3f902d8a-da0a-4116-9da7-823664a0b939" /> | — | після відпускання |

## Запуск

1. Відкрити проєкт у STM32CubeIDE.
2. Вибрати режим у `button.h`.
3. **Project → Build Project**.
4. **Debug**, потім **Resume**, і додати змінні в Live Expressions.
