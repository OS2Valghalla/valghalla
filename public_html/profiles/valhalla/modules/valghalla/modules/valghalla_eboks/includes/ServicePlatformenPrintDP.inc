<?php

/**
 * @file
 * The wrapper class to work with Serviceplatformen Print service.
 */

use Drupal\Valghalla\Pdf;
use Mpdf\Output\Destination;

module_load_include('inc', 'valghalla_eboks', 'includes/ServicePlatformenPrint');

/**
 * Class that holds the service requests for Digital Post.
 */
class ServicePlatformenPrintDP extends ServicePlatformenPrint {

  /**
   * MaterialeID variable.
   *
   * @var string
   *   Also used as MeddelelsesIndholdstypeIdentifikator or SysID.
   */
  private $materialeId;

  /**
   * ServiceplatformenPrintDP constructor.
   */
  public function __construct() {
    parent::__construct();

    $this->materialeId = variable_get('valghalla_eboks_materiale_id_dp');
  }

  /**
   * Set receiver callback.
   */
  public function setTo($to, $type = 'CPR') {
    $to = str_replace('-', '', $to);
    $this->request['BrevSPBody']['ForsendelseI']['ForsendelseModtager'] = array(
      'AfsendelseModtager' => array($type . 'nummerIdentifikator' => $to),
    );
  }

  /**
   * Prepare request data.
   *
   * @param array $params
   *   Request array.
   *
   * @throws Exception
   */
  public function prepareRequest(array &$params) {
    $params['rendered_message'] = $this->renderMessage($params);
    $pdf = new Pdf();
    $pdf->WriteHTML($params['rendered_message']);
    $pdf_output = $pdf->Output(FALSE, Destination::STRING_RETURN);
    $title_text = format_string($params['subject'], $params['tokens']);

    // TitelTekst xml field has max length 50 ch.
    if (drupal_strlen($title_text) > 50) {
      $title_text = mb_substr($title_text, 0, 47) . '...';
    }
    $params['title_text'] = $title_text;
    $this->request['BrevSPBody'] = array(
      'Kanalvalg' => 'D',
      'Prioritet' => 'D',
      'ForsendelseI' => array(
        'AfsendelseIdentifikator' => $this->getId($params),
        'FilformatNavn' => 'PDF',
        'MeddelelseIndholdData' => $pdf_output,
        'DokumentParametre' => array(
          'TitelTekst' => $params['title_text'],
        ),
        'DigitalPostParametre' => array(
          'MeddelelseIndholdstypeIdentifikator' => $this->materialeId,
          'MedsendDokumentRegistreringIndikator' => 0,
        ),
      ),
    );

    if (!empty($params['attachments'])) {
      $attachments = array();
      foreach ($params['attachments'] as $key => $attachment) {
        $attachments[] = array(
          'BilagNavn' => $attachment['filename'],
          'BilagSorteringsIndeksIdentifikator' => $key,
          'FilformatNavn' => $attachment['extension'],
          'VedhaeftningIndholdData' => $attachment['filecontent'],
        );
      }
      $this->request['BrevSPBody']['ForsendelseI']['BilagSamling'] = $attachments;
    }
  }

}
