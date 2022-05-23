(function ($) {
  Drupal.behaviors.valghallaExternalServerSignup = {
    attach: function (context, settings) {
      $(document).ajaxStart(function(){
        $('#edit-terms-agreement').attr("disabled", "disabled");
        $('#edit-submit').attr("disabled", "disabled");
      } );
      $(document).ajaxComplete(function(){
        // the code to be fired after the AJAX is completed
        if (!$('#edit-submit').hasClass("form-disabled")) {
          $('#edit-submit').removeAttr("disabled");
        }
        $('#edit-terms-agreement').removeAttr("disabled");

      });
    }
  };
})(jQuery);
