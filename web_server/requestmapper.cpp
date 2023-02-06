#include "requestmapper.h"
#include "httpsession.h"
#include "global.h"

RequestMapper::RequestMapper(QObject* parent)
    : HttpRequestHandler(parent) {
    // empty
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    //AJAX request
    if (path=="/parse_inc_data") {
        webRequestController.service(request, response);
    }
    else if (path.startsWith("/")) {
         //Show index page
         staticFileController->service(request,response);
    }
    else {
        response.setStatus(404,"Not found");
        response.write("The URL is wrong, no such document.",true);
    }

    qDebug("RequestMapper: finished request");
}
