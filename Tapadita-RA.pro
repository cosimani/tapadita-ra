#---------------------------------
#
# Proyecto Tapadita -version-tejo-
#
#---------------------------------

QT += core gui opengl network multimedia widgets sql

TEMPLATE = app

DEFINES += NO_DEBUG_ARUCO

DEFINES += CAMARAOPENCV #cuando este definida se enumeraran las camaras con Opencv

# La siguiente macro es para poder crear el Portable instalador. Cambia la carpeta del proyecto .. por .
#DEFINES += PORTABLE

unix:DIR_OPENCV_LIBS = /usr/local/lib

unix:INCLUDEPATH += "/usr/include/GL/"                             # OpenGL
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libglut.so"                # OpenGL

unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_core.so         # OpenCV
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_highgui.so      # OpenCV
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_imgproc.so      # OpenCV
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_objdetect.so    # OpenCV
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_calib3d.so      # OpenCV
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_ml.so           # OpenCV
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_contrib.so     # OpenCV

unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_video.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_features2d.so

unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_flann.so
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_gpu.so
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_legacy.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_ml.so
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_ocl.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_photo.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_stitching.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_superres.so
#unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_ts.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_video.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_videostab.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_imgcodecs.so
unix:LIBS += $$DIR_OPENCV_LIBS/libopencv_videoio.so

unix:LIBS += "/usr/lib/x86_64-linux-gnu/lib3ds.so"                 # Modelos 3D



win32:DIR_OPENCV_LIBS = C:/Qt/OpenCV-3.1.0

win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/core/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/imgproc/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/video/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/flann/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/features2d/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/calib3d/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/legacy/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/objdetect/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/highgui/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/photo/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/ml/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/contrib/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/hal/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/imgcodecs/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/videoio/include"

win32:LIBS += -L"$$DIR_OPENCV_LIBS/opencv/compilado/lib"

win32:LIBS += -lopencv_core310.dll
win32:LIBS += -lopencv_highgui310.dll
win32:LIBS += -lopencv_imgproc310.dll
win32:LIBS += -lopencv_objdetect310.dll
win32:LIBS += -lopencv_calib3d310.dll

win32:LIBS += -lopencv_ml310.dll
win32:LIBS += -lopencv_video310.dll
win32:LIBS += -lopencv_features2d310.dll
win32:LIBS += -lopencv_flann310.dll
win32:LIBS += -lopencv_photo310.dll
win32:LIBS += -lopencv_stitching310.dll
win32:LIBS += -lopencv_superres310.dll
win32:LIBS += -lopencv_video310.dll
win32:LIBS += -lopencv_videostab310.dll
win32:LIBS += -lopencv_imgcodecs310.dll
win32:LIBS += -lopencv_videoio310.dll

win32:LIBS += -lopengl32
win32:LIBS += -lglu32

# A continuacion los archivos de cabecera de GLUT
win32:INCLUDEPATH += "C:/Qt/glut-3.7.6/include/GL"
# Aqui la carpeta donde se encuentran los archivos de libreria .a de GLUT
win32:LIBS += -L"C:/Qt/glut-3.7.6/lib/glut"
# A continuacion se indican cada uno de los archivos de libreria .a
# Donde se indica -lglut32 es porque el archivo se llama libglut32.a
win32:LIBS += -lglut32

# A continuacion los archivos de cabecera de Lib3ds
win32:INCLUDEPATH += "C:/Qt/lib3ds/include"
# Aqui la carpeta donde se encuentra el archivo de libreria .a de lib3ds
win32:LIBS += -L"C:/Qt/lib3ds/lib"
# A continuacion se indican elos archivo de libreria .a
# Donde se indica -lglut32 es porque el archivo se llama libglut32.a
win32:LIBS += -l3ds




SOURCES += main.cpp\
           view/scene.cpp \
           aruco/ar_omp.cpp \
           aruco/arucofidmarkers.cpp \
           aruco/board.cpp \
           aruco/boarddetector.cpp \
           aruco/cameraparameters.cpp \
           aruco/highlyreliablemarkers.cpp \
           aruco/marker.cpp \
           aruco/markerdetector.cpp \
           aruco/subpixelcorner.cpp \
    model/database.cpp \
    view/viewcontroller.cpp \
    view/registerplayer.cpp \
    view/mibutton.cpp \
    view/image.cpp \
    view/profilepicture.cpp \
    view/micampotexto.cpp \
    view/inicio.cpp \
    view/principal.cpp \
    view/miqlineedit.cpp \
    factory.cpp \
    controller/cvdrawing.cpp \
    model/jugador.cpp

HEADERS += controller/model.h \
           view/scene.h \
           controller/texture.h \
           video.h \
           aruco/ar_omp.h \
           aruco/aruco.h \
           aruco/arucofidmarkers.h \
           aruco/board.h \
           aruco/boarddetector.h \
           aruco/cameraparameters.h \
           aruco/exports.h \
           aruco/highlyreliablemarkers.h \
           aruco/marker.h \
           aruco/markerdetector.h \
           aruco/subpixelcorner.h \
    view/logo.h \
    model/database.hpp \
    common.h \
    view/viewcontroller.h \
    view/registerplayer.h \
    view/mibutton.h \
    view/image.h \
    view/profilepicture.h \
    view/micampotexto.h \
    view/inicio.h \
    view/principal.h \
    view/miqlineedit.h \
    factory.h \
    controller/cvdrawing.h \
    model/jugador.h

FORMS += \
    view/viewcontroller.ui \
    view/registerplayer.ui \
    view/profilepicture.ui \
    view/micampotexto.ui \
    view/inicio.ui \
    view/principal.ui

DISTFILES += \
    README.md \
    Files/db.sqlite \
    Files/CameraParameters.yml \
    images/empty_profile.jpg \
    images/empty_profile2.jpg \
