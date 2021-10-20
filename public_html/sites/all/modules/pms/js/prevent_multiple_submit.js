/**
 * @file
 * Small JS code, prevent user to click multiple times.
 *
 */
(function ($) {
  'use strict';
  Drupal.behaviors.prevent_multiple_submit = {
    attach: function (context, settings) {
      jQuery('form').submit(function () {
        jQuery(this).submit(function (e) {
          e.preventDefault();
        });
        return true;
      });
    }
  };
})(jQuery);
