
![Store-page:](https://github.com/zhuyaliang/images/blob/master/app-store-001.png)
![Local-page:](https://github.com/zhuyaliang/images/blob/master/app-store-003.png)
![Details-page:](https://github.com/zhuyaliang/images/blob/master/app-store-004.png)
![Store-list-page:](https://github.com/zhuyaliang/images/blob/master/app-store-002.png)
# Explain

Refer to gnome-software interface, write a simple and portable software management interface.You can view and download the software provided by the App Store, and manage existing local applications.At present, the application store server is not ready for open source, so it can only use the local application management function.
```
/etc/soft-app-store/appstore-server.ini
```
Setting up the server IP address and port number, we can only use the unique address we provide at present.

## Code reference

https://github.com/GNOME/gnome-software/

## Compile

```
meson build -Dprefix=/usr
ninja -C build
sudo ninja -C build install


