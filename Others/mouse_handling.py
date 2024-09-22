import pyautogui
import time

#Turn supply on
pyautogui.moveTo(350, 1285, duration = 1)
pyautogui.click(350, 1285)

#Main power
pyautogui.moveTo(200, 1670, duration = 1)
pyautogui.click(200, 1670)

i = 1

while i <= 10:
    #Record recording
    pyautogui.moveTo(100, 1190, duration = 0.5)
    pyautogui.click(100, 1190)

    #Recording time
    time.sleep(2)

    #Stop recording
    pyautogui.moveTo(130, 1190, duration = 0)
    pyautogui.click(130, 1190)

    #Left click on the recording
    pyautogui.moveTo(200, 1950, duration = 0.5)
    pyautogui.click(button='right', x=200, y=1950)

    #Export to CVS
    pyautogui.moveTo(250, 1930, duration = 0.5)
    pyautogui.click(250, 1930)

    #Otii is ready
    pyautogui.moveTo(1000, 1150, duration = 0.5)
    pyautogui.click(1000, 1150)

    name = 'recording'+str(i)

    #Type the name of the recording
    pyautogui.typewrite(name)

    #Save
    pyautogui.moveTo(1550, 1180, duration = 0.5)
    pyautogui.click(1550, 1180)

    #Left click on the recording
    pyautogui.moveTo(200, 1950, duration = 0.5)
    pyautogui.click(button='right', x=200, y=1950)

    #Delete recording
    #pyautogui.moveTo(250, 1820, duration = 0.5)
    #pyautogui.click(250, 1820)

    i = i + 1
