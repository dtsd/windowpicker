TARGET=windowpicker

ORGANIZATION_NAME= "dtsd.ru"
ORGANIZATION_DOMAIN = "dtelsenko@gmail.com"

APPLICATION_NAME = "windowpicker"

APPLICATION_VERSION_MAJOR = 1
APPLICATION_VERSION_MINOR = 2
APPLICATION_VERSION_PATCH = 0
APPLICATION_VERSION = "$${APPLICATION_VERSION_MAJOR}.$${APPLICATION_VERSION_MINOR}.$${APPLICATION_VERSION_PATCH}""

DEFINES += \
	ORGANIZATION_NAME=\\\"$${ORGANIZATION_NAME}\\\" \
	ORGANIZATION_DOMAIN=\\\"$${ORGANIZATION_DOMAIN}\\\" \
	APPLICATION_NAME=\\\"$${APPLICATION_NAME}\\\" \
	APPLICATION_VERSION=\\\"$${APPLICATION_VERSION}\\\" \
	APPLICATION_VERSION_MAJOR=$${APPLICATION_VERSION_MAJOR} \
	APPLICATION_VERSION_MINOR=$${APPLICATION_VERSION_MINOR} \
	APPLICATION_VERSION_PATCH=$${APPLICATION_VERSION_PATCH} 

