(function ($) {

    $(window).load(function () {
        var input = $('#edit-field-valghalla-mail-attachments-und-0-upload');
        input.addClass('hidden');

        $('#edit-field-valghalla-mail-attachments-und-0-upload-button').addClass('hidden');
        $(":file").filestyle({buttonText: "Vælg fil"});

        $(":file").filestyle({placeholder: "Ingen fil"});

    });

})(jQuery);
