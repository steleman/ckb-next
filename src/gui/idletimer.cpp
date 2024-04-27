#include "idletimer.h"
#include <stdint.h>
#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QWindow>

#ifdef USE_XCB_SCREENSAVER
#include <QX11Info>
#include <xcb/xcb.h>
#include <xcb/screensaver.h>
#include <limits.h>

#include "mainwindow.h"

int IdleTimer::getIdleTime(){
    const MainWindow* const mw = MainWindow::mainWindow;
    if(!mw || !mw->windowHandle())
        return 0;
    xcb_connection_t* conn = QX11Info::connection();

    xcb_get_geometry_cookie_t geomCookie = xcb_get_geometry(conn, mw->windowHandle()->winId());
    xcb_get_geometry_reply_t* geomReply = xcb_get_geometry_reply(conn, geomCookie, nullptr);

    xcb_screensaver_query_info_cookie_t replyCookie = xcb_screensaver_query_info(conn, geomReply->root);
    xcb_screensaver_query_info_reply_t* replyInfo = xcb_screensaver_query_info_reply(conn, replyCookie, nullptr);

    uint32_t userIdle = replyInfo->ms_since_user_input;
    free(replyInfo);

    // Clamp the value
    if(userIdle > INT_MAX)
        userIdle = INT_MAX;

    return (int)userIdle;
}

#else

int IdleTimer::getIdleTime(){
    return 0;
}
#endif

bool IdleTimer::isWayland(){
    QByteArray wdesktop = qgetenv("WAYLAND_DESKTOP");
    if(!wdesktop.isEmpty())
        return true;
    QString stype(qgetenv("XDG_SESSION_TYPE"));
    if(stype == "wayland")
        return true;
    return false;
}
