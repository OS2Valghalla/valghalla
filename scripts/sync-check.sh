#!/usr/bin/env bash

drush @sites --root=/var/www/valghalla-internal/public_html valg-isc -y > /tmp/valgha-sync-check.log 2>&1
mail -s "Daily valghalla sync check status" ay@bellcom.dk -a "From: Valghalla <domain@bellcom.dk>" < /tmp/valgha-sync-check.log
