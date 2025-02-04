curl https://registry.khronos.org/OpenGL/api/GL/wglext.h --ssl-no-revoke -o wglext.h
curl https://registry.khronos.org/OpenGL/api/GL/glext.h --ssl-no-revoke -o glext.h
if not exist KHR mkdir KHR
pushd KHR
curl https://registry.khronos.org/EGL/api/KHR/khrplatform.h --ssl-no-revoke -o khrplatform.h
popd
python gengl.py glext.h GetAnyGLFuncAddress