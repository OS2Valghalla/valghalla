#!/usr/bin/env bash

drush @sites --root=/var/www/valghalla-internal/public_html ev "if (function_exists('odin_volunteers_full_counts')) {odin_volunteers_full_counts(1);}" -y > /tmp/valgha-counters-check.log 2>&1
#drush @sites --root=/var/www/valghalla-external/public_html ev "if (function_exists('odin_volunteers_full_counts')) {odin_volunteers_full_counts(1);}" -y >> /tmp/valgha-counters-check.log 2>&1
mail -s "Daily valghalla counter check status" ay@bellcom.dk -a "From: Valghalla <domain@bellcom.dk>" < /tmp/valgha-counters-check.log
