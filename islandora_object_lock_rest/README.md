CONTENTS OF THIS FILE
---------------------

 * summary
 * requirements
 * installation
 * configuration
 * documentation
 * troubleshooting

SUMMARY
-------

Islandora Object Lock REST provides a REST interface to object locking.

REQUIREMENTS
------------

Islandora Object Lock

Islandora REST

INSTALLATION
------------

Download the module to sites/all/modules or equivalent and enable the module.

CONFIGURATION
-------------

Visit /admin/islandora/object-lock to set the default lock duration.

DOCUMENTATION
-------------

Refer to the Islandora REST readme for general response codes and background.

## Get object lock status

#### URL syntax
islandora/rest/v1/object/{pid}/lock

#### HTTP Method
GET

#### Headers
Accept: application/json

#### Get Parameters
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pid           | Persistent identifier of the object

#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| locked        | Boolean if the object is locked or not
| expires       | The expiration datetime of the lock if it exists
| user          | The username of the lock owner

#### Example Response
```JSON
{
"locked":true,
"expires":"2013-08-21T19:42:52Z",
"user":"admin"
}
```

## Create an object lock

#### URL syntax
islandora/rest/v1/object/{pid}/lock

#### HTTP Method
POST

#### Headers
Accept: application/json

#### Get Parameters
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pid           | Persistent identifier of the object

#### Response: 201 Created

## Remove an object lock

#### URL syntax
islandora/rest/v1/object/{pid}/lock

#### HTTP Method
DELETE

#### Headers
Accept: application/json

#### Get Parameters
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pid           | Persistent identifier of the object

#### Response: 200 OK
