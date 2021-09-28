<?php

namespace ValghallaInternalServer;

/**
 * Handles the connection to external server webservice.
 */
class ExternalWebservice {

  /**
   * Web service endpoint.
   *
   * @var string
   */
  private $endpoint;

  /**
   * Authorization token.
   *
   * @var string
   */
  private $authToken;

  /**
   * Fetches the endpoint URL and puts it into a variable.
   */
  public function __construct() {
    $this->endpoint = variable_get('valghalla_external_server_endpoint');
  }

  /**
   * Checks if the webservice can be accessed successfully.
   *
   * Fetches the checksum, decrypts is and checks if it a "success" word.
   *
   * @return bool
   *   TRUE or FALSE.
   */
  public function heartbeat() {
    $checksum = NULL;
    $fetchedContent = $this->fetchContent();

    if (is_array($fetchedContent)) {
      foreach ($fetchedContent as $content) {
        if (isset($content->checksum)) {
          $checksum = $content->checksum;
        }
      }
    }
    $decryptedText = valghalla_synch_node_export_get_decrypt($checksum, variable_get('valghalla_external_server_hash_salt'));

    return (strcasecmp($decryptedText, 'success') === 0);
  }

  /**
   * Calling webservice endpoint: /valghalla_resource_export.
   *
   * @param int $page
   *   Number of page to return.
   *
   * @return array
   *   Result array.
   */
  public function fetchContent($page = 0) {
    $getParams = http_build_query(
      array(
        'page' => $page,
      )
    );
    $requestUrl = $this->endpoint . '/valghalla_resource_export.json?' . $getParams;
    $result = $this->requestWrapper($requestUrl);
    return $result;
  }

  /**
   * Calling webservice endpoint : POST /valghalla_resource_export.
   *
   * @param string $data
   *   Serialized node data.
   *
   * @return array
   *   Contains an array of
   *   translated strings to be shown to the user as messages
   */
  public function pushNodeSerialized($data) {
    $requestUrl = $this->endpoint . '/valghalla_resource_export.json';

    $options = array();
    $options['method'] = 'POST';
    $options['data'] = json_encode(array(
      'node_export_data' => $data,
    ));

    $result = $this->requestWrapper($requestUrl, $options);
    return $result;
  }

  /**
   * Calling webservice endpoint : POST /taxonomy_term.
   *
   * Also checks if the term with the same uuid is already present in the
   * system, then it will be updated. If not - new term will be created.
   *
   * @param mixed $term
   *   The term object.
   *
   * @return int
   *   1 - is term was created, 2 - term was updated.
   */
  public function pushTerm($term) {
    // Check if the term exists on external server.
    $remoteTerm = $this->getTermByUuid($term->uuid);
    if ($remoteTerm) {
      // There is the same term on remote server, update it.
      $term->tid = $remoteTerm->tid;
    }
    else {
      // No term on remote server with the same uuid, create a new one.
      unset($term->tid);
    }

    $json = valghalla_internal_server_term_to_json($term);

    $requestUrl = $this->endpoint . '/taxonomy_term.json';

    $options = array();
    $options['method'] = 'POST';
    $options['data'] = $json;
    $result = $this->requestWrapper($requestUrl, $options);

    if (!empty($result) && is_array($result)) {
      return array_pop($result);
    }
  }

  /**
   * Calling webservice endpoint : GET /taxonomy_term.
   *
   * @param string $uuid
   *   Uuid of the term.
   *
   * @return mixed
   *   Taxonomy term object.
   */
  public function getTermByUuid($uuid) {
    $params = http_build_query(array(
      'parameters' => array(
        'uuid' => $uuid,
      ),
    ));

    $requestUrl = $this->endpoint . '/taxonomy_term.json?' . $params;
    $result = $this->requestWrapper($requestUrl);

    if (!empty($result) && is_array($result)) {
      return array_pop($result);
    }
  }

  /**
   * Calling webservice endpoint: DELETE /taxonomy_term/tid.
   *
   * @param string $uuid
   *   Uuid of the term.
   *
   * @return int
   *   3 - is term was deleted..
   */
  public function deleteTerm($uuid) {
    $remoteTerm = $this->getTermByUuid($uuid);

    if ($remoteTerm) {
      $remoteTermTid = $remoteTerm->tid;

      $options = array();
      $options['method'] = 'DELETE';

      $requestUrl = $this->endpoint . '/taxonomy_term/' . $remoteTermTid . '.json';
      $result = $this->requestWrapper($requestUrl, $options);

      if (!empty($result) && is_array($result)) {
        return array_pop($result);
      }
    }
  }

  /**
   * Calling webservice endpoint : GET /node.
   *
   * @param string $uuid
   *   Uuid of the node.
   *
   * @return mixed
   *   Node object.
   */
  public function getNodeByUuid($uuid) {
    $params = http_build_query(array(
      'parameters' => array(
        'uuid' => $uuid,
      ),
    ));

    $requestUrl = $this->endpoint . '/node.json?' . $params;
    $result = $this->requestWrapper($requestUrl);

    if (!empty($result) && is_array($result)) {
      return array_pop($result);
    }
  }

  /**
   * Calling webservice endpoint : DELETE /node/nid.
   *
   * @param string $uuid
   *   Uuid of the node.
   *
   * @return bool
   *   TRUE if node has been deleted.
   */
  public function deleteNode($uuid) {
    $remoteNode = $this->getNodeByUuid($uuid);

    if ($remoteNode) {
      $remoteNodeNid = $remoteNode->nid;

      $options = array();
      $options['method'] = 'DELETE';

      $requestUrl = $this->endpoint . '/node/' . $remoteNodeNid . '.json';
      $result = $this->requestWrapper($requestUrl, $options);

      if (!empty($result) && is_array($result)) {
        return array_pop($result);
      }
    }
  }

  /**
   * Calling webservice endpoint : GET /valghalla_resource_export/uuid.
   *
   * @param string $uuid
   *   Uuid of the resource.
   *
   * @return string
   *   Serialized object formatted as string.
   */
  public function exportByUuid($uuid) {
    $requestUrl = $this->endpoint . '/valghalla_resource_export/' . $uuid . '.json';
    $result = $this->requestWrapper($requestUrl);

    if (!empty($result) && is_array($result)) {
      return array_pop($result);
    }
  }

  /**
   * Calling webservice endpoint : DELETE /valghalla_resource_export/uuid.
   *
   * @param string $uuid
   *   Uuid of the resource.
   *
   * @return bool
   *   Always TRUE.
   */
  public function removeFromQueueByUuid($uuid) {
    $requestUrl = $this->endpoint . '/valghalla_resource_export/' . $uuid . '.json';

    $result = $this->requestWrapper($requestUrl, array('method' => 'DELETE'));

    if (!empty($result) && is_array($result)) {
      return array_pop($result);
    }
  }

  /**
   * Helper function.
   *
   * Wraps the request, adds authorization string and performs
   * an HTTP request call.
   *
   * @param string $requestUrl
   *   The request URL.
   * @param mixed $options
   *   Array of the options, @see drupal_http_request.
   *
   * @return string
   *   The result of on success, error message on failure.
   */
  private function requestWrapper($requestUrl, $options = array()) {
    if (!$this->authToken) {
      $user = variable_get('valghalla_external_server_user');
      $password = variable_get('valghalla_external_server_password');
      $this->authToken = 'Basic ' . base64_encode($user . ':' . $password);
    }

    $options['headers'] = array();
    $options['headers']['Authorization'] = $this->authToken;
    $options['headers']['Content-Type'] = 'application/json';
    $options['timeout'] = 180;

    $result = drupal_http_request($requestUrl, $options);

    if ($result->code == 200) {
      // Check if the string is in JSON format.
      if (is_string($result->data) && is_array(json_decode($result->data, TRUE))) {
        return json_decode($result->data);
      }
      else {
        return $result->data;
      }
    }
    else {
      return $result->error;
    }
  }

}
