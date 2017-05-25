Samples repository

IOx Services Samples depend on IOx Services SDK which is a set of
libraries/infrastructure than enable micro service ecosystem.

For packaging these services/apps, there are two ways

- Docker enabled SDE, the samples container docker files needed to pull
artifacts from devhub and create docker style containers.

- Native LXC containers, Bundling the service as a LxC container, please refer to the
IOx SDK documentation. IOx SDK is a bunch of tool chains and utilities
needed to create a IOx installable, that can result in bringing up LxC
on a Iox enabled device.

- IOx Servics can be deployed using 'ioxclient' only as of now and Fog
Director does not support services as yet.
