var volunteer_info = volunteer_info || {};
var volunteer_tmp_element;
var valghalla_volunteers = valghalla_volunteers || [];

(function ($) {
  Drupal.behaviors.valghalla_volunteers = {
    attach: function (context, settings) {

      // Fetch info about post.
      $('.entity-list--volunteer-form input').on('focus', function () {
        var $parent = $(this).parent().parent();
        var $el = $parent.find('.js-add-volunteer');
        volunteer_info.post_id = $parent.attr('data-post');
        volunteer_info.pollingstation_nid = $el.attr('data-pollingstation_nid');
        volunteer_info.role_nid = $el.attr('data-role_nid');
        volunteer_info.party_tid = $el.attr('data-party_tid');
        volunteer_info.validate_citizenship = $el.attr('data-validate-citizenship');
        volunteer_info.validate_municipality = $el.attr('data-validate-municipality');
        volunteer_info.validate_civil_status = $el.attr('data-validate-civil-status');
        volunteer_tmp_element = $parent;

        $(this).autocomplete({
          minLength: 0,
          source: function( request, response ) {
            var matcher = new RegExp( $.ui.autocomplete.escapeRegex( request.term ), "i" );
            response( $.grep( valghalla_volunteers, function( value ) {
                if (volunteer_info.validate_citizenship && !value.citizenship) {
                    return false;
                }
                if (volunteer_info.validate_municipality && !value.municipality) {
                    return false;
                }

                if (volunteer_info.validate_civil_status && !value.civil_status) {
                    return false;
                }

                value = value.label || value.value || value;
                return matcher.test( value );
          }))},
          focus: function (event, ui) {
            return false;
          },
          select: function (event, ui) {
            ui.item.volunteer_item = volunteer_tmp_element;
            Drupal.behaviors.valghalla_volunteers.autocompleteSelect(ui.item);

            return false;
          }
        })
            .data("ui-autocomplete")._renderItem = function (ul, item) {
          return $("<li>")
              .append("<a data-volunteer_nid='" + item.volunteer_nid + "'>" + item.label + "<br>" + item.desc + "</a>")
              .appendTo(ul);
        };
      });

      // Add to polling station modal: ---------------------->
      $('.js-add-volunteer').on('click', function () {
        var $parent = $(this).parent();
        var $el = $(this);

        volunteer_info.post_id = $parent.attr('data-post');
        volunteer_info.pollingstation_nid = $el.attr('data-pollingstation_nid');
        volunteer_info.role_nid = $el.attr('data-role_nid');
        volunteer_info.party_tid = $el.attr('data-party_tid');
        volunteer_info.validate_citizenship = $el.attr('data-validate-citizenship');
        volunteer_info.validate_municipality = $el.attr('data-validate-municipality');
        volunteer_info.validate_civil_status = $el.attr('data-validate-civil-status');

        $(document).trigger('volunteersLoaded');

        $('.js-add-volunteer-modal').modal();
      });

      // Remove volunteer from post.
      $('.js-remove-volunteer').on('click', function () {
        var fcid = $(this).attr('data-fcid');
        $parent = $(this).parent();

        // Show a modal.
        swal({
              title: Drupal.t('Er du sikker?'),
              text: Drupal.t('Dette vil fjerne deltageren fra pladsen. Denne handling kan ikke fortrydes.'),
              type: 'warning',
              showCancelButton: true,
              confirmButtonClass: 'btn-danger',
              cancelButtonText: Drupal.t('Annullér'),
              confirmButtonText: Drupal.t('Ja, fjern deltageren'),
              closeOnConfirm: false,
              showLoaderOnConfirm: true
            },
            function () {
              $.post('/ajax/volunteers/station/remove', {'fcid': fcid}, function (data) {

                // Show a modal.
                swal({
                  title: Drupal.t('Deltageren blev fjernet fra pladsen!'),
                  text: Drupal.t('Siden genindlæses...'),
                  type: 'success',
                  showCancelButton: false,
                  showConfirmButton: false,
                });

                // Refresh after 2.5 sec.
                setTimeout(function () {
                  location.reload();
                }, 2500);
              });
            });
      });

      // Select volunteer from modal.
      $('.modal').on('click', '.js-select-volunteer', function (event) {
        var $button = $(this);
        var $icon = $('<i />').addClass('fa fa-refresh fa-spin fa-fw');
        $button.attr('disabled', true);
        $button.append($icon);

        volunteer_info.volunteer_nid = $(this).attr('data-volunteer_nid');

        $.post('/ajax/volunteers/station/add', volunteer_info, function (data) {
          $button.attr('disabled', false);

          // Hide modal.
          $('.modal').modal('hide');

          // When modal is hidden.
          $('.modal').on('hidden.bs.modal', function () {

            // Show a modal.
            swal({
              title: Drupal.t('Deltageren blev tilføjet til pladsen!'),
              text: Drupal.t('Siden genindlæses...'),
              type: 'success',
              showCancelButton: false,
              showConfirmButton: false,
            });

            // Refresh after 2.5 sec.
            setTimeout(function () {
              location.reload();
            }, 2500);
          })
        });
      });

      Drupal.behaviors.valghalla_volunteers.populateTable();
      // <------------------- Add to polling station modal
    },
    autocompleteSelect: function (item) {
      volunteer_info.volunteer_nid = item.volunteer_nid;

      item.volunteer_item
        .addClass('waiting')
        .find('input').addClass('waiting')
        .attr('disabled', 'disabled');

      // Add volunteer.
      $.post('/ajax/volunteers/station/add', volunteer_info, function (data) {

        item.volunteer_item
          .removeClass('waiting')
          .find('input').removeClass('waiting')
          .removeAttr('disabled');

        // Show a modal.
        swal({
          title: Drupal.t('Deltageren blev tilføjet til pladsen!'),
          text: Drupal.t('Siden genindlæses...'),
          type: 'success',
          showCancelButton: false,
          showConfirmButton: false,
        });

        // Refresh after 2.5 sec.
        setTimeout(function () {
          location.reload();
        }, 2500);
      });
    },
    populateTable: function () {
      $.get('/ajax/volunteers/station/getvolunteers', function (data) {

        valghalla_volunteers = [];
        for (var key in data) {

          valghalla_volunteers.push({
            label: "(" + data[key].volunteer_party + ") " + data[key].volunteer_name,
            value: "(" + data[key].volunteer_party + ")" + data[key].volunteer_name,
            volunteer_nid: data[key].volunteer_nid,
            desc: "",
            citizenship: data[key].citizenship,
            civil_status: data[key].civil_status,
            municipality: data[key].municipality
          });
        }

        $(document).trigger('volunteersLoaded');
      });
    },
    unsetVolunteer: function (nid) {
      var data = Drupal.settings.valghalla_volunteers.volunteers;
      for (var key in data) {
        if (data[key].volunteer_nid === nid) {
          Drupal.settings.valghalla_volunteers.volunteers.splice(key, 1);
        }
      }
    }
  };
  Drupal.behaviors.valghallaMoveEditPollingStationButton = {
    attach: function (context, settings) {
      var $button = $('.edit-polling-station');
      var $wrapper = $('.page-volunteers-station .breadcrumb__wrapper');
      var $breadcrumb = $('.breadcrumb');

      // If we don't have a wrapper, stop what we are doing!
      if ($wrapper.length === 0) {
        return;
      }

      // Don't proceed if a breadcrumb is present. This will break styling.
      if ($breadcrumb.length > 0) {
        return;
      }

      // Inserts a button into the wrapper.
      $wrapper.html($button.removeClass('hidden'));
    },
  };
  Drupal.behaviors.valghallaLoadSeatMatrixRolesTable = {
    attach: function (context, settings) {
      var basePath = settings.basePath;
      var polling_station_id = settings.valghalla_volunteers.seat_matrix_polling_station;

      $.get( basePath + "ajax/seat_matrix_roles_table/" + polling_station_id, function( data ) {
        $('.ajax-lazy-seat-matrix-roles-table').html(data);
      });
    }
  }
})(jQuery);
