Samples repository

IOx Services Samples depend on IOx Services SDK which is a set of
libraries/utilities that enable IOx Microservice ecosystem.

For packaging these services/apps, there are two ways

- Docker style containers using Docker enabled SDE. 
The samples contain docker files needed to pull
artifacts from devhub and create docker style images. Using ioxclient
these docker style images can be converted to IOX installables and can
be brought up as containers on IOx enabled devices.

- Native LXC containers using IOx SDK, for bundling the service as a LxC container, 
please refer to the IOx SDK documentation. IOx SDK is a bunch of tool
chains and utilities needed to create a IOx installable, that can result
in bringing up LxC on a Iox enabled device.

- IOx Servics can be deployed using 'ioxclient CLI' ONLY as of now and Fog
Director does not support deploying/managing IOx services as yet.
