# tcp_modbus_web_server

Application for messaging and events over Modbus TCP.

The project uses the FreeMODBUS library.

An example project on FreeMODBUS-TCP with a master and two slaves. Each of the devices has a web interface for user interaction.

The **web_server** directory contains the source codes and the project for QT

This repository contains compiled projects for windows:

* release_final_master - TCP master
* release_final_slave_1 - TCP slave 1
* release_final_slave_2 - TCP slave 2

The etc/webapp1.ini file in projects allows you to configure the launched application, configure ports, ip for slave devices, etc.

***Note: You can't make a slave from a master via a configuration file (the reverse is also true)***

Video of work (clickable):


---

Приложение для обмена сообщениями и событиями поверх Modbus TCP.

Проект использует библиотеку FreeMODBUS. 

Пример проекта на FreeMODBUS-TCP с мастером и двумя подчиненными устройтсвами. Каждое из устройств имеет веб-интерфейс для взаимодействия с пользователем. 

В директории **web_server** находятся исходные коды и проект для QT

В данном репозитории содержаться собранные проекты для windows:

* release_final_master - TCP мастер
* release_final_slave_1 - TCP подчиненный 1
* release_final_slave_2 - TCP подчиненный 2

Файл etc/webapp1.ini в проектах позволяет провести конфигурацию запускаемого приложения, настроить порты, ip у подчиненных устройств и прочее

***Примечание: Из мастера не сделать подчиненное устройство через конфигурационный файл (верно и обратное)***

Видео работы (кликабельно):

[![video1](https://img.youtube.com/vi/57tRBdkFqdo/0.jpg)](https://www.youtube.com/watch?v=57tRBdkFqdo)

