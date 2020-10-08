# Telegram_Bot_Consumable_Monitoring

    CONCEPT : Use a telegram bot to receive a message as ESP8266 starts up,
    repeat at a set time and sleep in between messages to save battery.
    NOTE : this works one way, only sending messages, any response is ignored.
    ****************************************************************
    
    Telegram Bot library written by Brian Lough and ammended by Bolukan
    YouTube:  https://www.youtube.com/brianlough
    Tindie:   https://www.tindie.com/stores/brianlough/
    Twitter:  https://twitter.com/witnessmenow
    github:   https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
    github:   https://github.com/bolukan

    ****************************************************************

    USE CASE : Generate an alert via Telegram when a consumable has run out
    by closing a hardware switch that powers up an ESP8266. The battery
    voltage level can be measured and also sent, together with the
    statically set minimum battery level.

    ****************************************************************

    USER VARIABLES : You need to use your WiFi SSID and password as well as
    the bot token and chat id (see below).

    ****************************************************************

    TELEGRAM BOT : You need to make a bot first to be able to use this. 
    Search 'botfather' and you will find plenty information on how to
    create your own telegram bot.
    - Botfather will give out your unique bot token when he creates the
    bot for you.
    - Use @myidbot (IDBot) in Telegram or the HTTPS request method to
    find out the chat ID of your bot.
    - Also note that you need to "start" your bot before it will do
    anything:
    In Telegram type @botfather to open a chat. Type /mybots to show
    and pick your bot(s).
    From your bot, type /start to activate your bot. Type /stop
    to de-activate it.

    ****************************************************************

    HARDWARE REQUIREMENTS :
    - For the ESP8266 to wake from deepsleep, it needs a connection
    between RST and A0 (or whatever is your ADC pin)
    - To be able to measure VCC you need to connect a resistor from VCC to A0.
    The value of this resistor depends on wether there already is a voltage divider
    in place like on the D1 mini and NodeMCU, or if it is a barebone ESP. For
    example; D1 mini measuring ~5V it should be 180k ohm. Alternatively
    you can use a multiturn potentiometer to exactly dial in the R value.

    ****************************************************************

    IMPORTANT : with the 'DeepSleep' pin open it is very hard to (re)flash
    as the ESP will go into deepsleep before you can upload the code.
    So, to re-flash connect this pin to GND. However, it cannot be
    connected to GND during boot as this will prevent the ESP from
    booting up. So, reset and then connect.

    ****************************************************************

