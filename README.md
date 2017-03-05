#Control de volumen con CORE AUDIO API (windows API)

Para control del volumen del sistema se debe acceder a la librería COM de windows, la interfaz encargada es
IAudioEndpointVolume, para esto:

* Iniciar la librería COM.
* Obtener puntero a la interfaz IMMDeviceEnumerator.
* Mediante lo ultimo, obtener puntero a la interfaz IMMDevice.
* Mediante lo ultimo, obtener un puntero IAudioEndpointVolume
* Esta ultima interfaz nos brindara los eventos para silenciar, cambiar volumen, y verificar estado del volumen.
