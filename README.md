CONTENTS OF THIS FILE
---------------------

 * summary
 * requirements
 * installation
 * configuration
 * troubleshooting

SUMMARY
-------

Islandora Object Lock allows users to lock objects to prevent modifications by
other users. It also automatically locks objects when edits are being made to
datastreams through the XML Form Builder.

REQUIREMENTS
------------

Islandora

Islandora XACML API

INSTALLATION
------------

Download the module to sites/all/modules or equivalent and enable the module.

CONFIGURATION
-------------

Visit /admin/islandora/object-lock to set the default lock duration.

TROUBLESHOOTING
---------------

Drupal user #1 is required to be in place to unlock objects during cron when
the locks have expired.  To view the delete object locks list one may have to
have an 'administrator' role.
