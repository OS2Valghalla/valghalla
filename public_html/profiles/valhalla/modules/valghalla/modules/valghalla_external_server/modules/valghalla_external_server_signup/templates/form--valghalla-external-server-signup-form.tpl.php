<?php

/**
 * @file
 * External signup form template.
 */

$comment_field = $form['comment'];
$submit = $form['submit'];
$term_agreement = $form['terms_agreement'];
if (isset($form['nemid_login_link'])) {
  $nemid_login_link = $form['nemid_login_link'];
  unset($form['nemid_login_link']);
}

if (isset($form['nemid_logout_link'])) {
  $nemid_logout_link = $form['nemid_logout_link'];
  unset($form['nemid_logout_link']);
}

unset($form['comment']);
unset($form['submit']);
unset($form['terms_agreement']);
?>

<div class="narrow-wrapper">

  <!-- Begin - message -->
  <?php if (isset($form['#party_message'])): ?>
    <div class="alert alert-info">
      <?php print $form['#party_message']; ?>
    </div>
  <?php endif; ?>
  <!-- End - message -->

  <!-- Begin - upper text -->
  <div class="partial">
    <div class="partial__body">
      <?php print $form['#upper_text']; ?>
    </div>
  </div>
  <!-- End - upper text -->

  <!-- Begin - nemid login -->
  <?php if ($form['#cpr_provider'] == 'nemlogin') : ?>
    <div class="partial">
      <?php if (isset($form['#cpr']) && !empty($form['#cpr'])) : ?>
        <p>Du er logget ind med NemID med følgende CPR nummer <?php print $form['#cpr'];?></p>
        <?php print drupal_render($nemid_logout_link); ?>
      <?php else: ?>
        <p>Login med NemLogin for at forsætte</p>
        <?php print drupal_render($nemid_login_link); ?>
      <?php endif; ?>
    </div>
  <?php endif ?>
  <!-- End - nemid login -->

  <!-- Begin - form -->
  <div class="partial">
    <div class="partial__body">
      <?php print drupal_render_children($form); ?>
    </div>
  </div>
  <!-- End - form -->

  <!-- Begin - bottom text -->
  <div class="partial">
    <div class="partial__body">
      <?php print $form['#bottom_text']; ?>
    </div>
  </div>
  <!-- End - bottom text -->

  <!-- Begin - comment -->
  <?php print drupal_render($comment_field); ?>
  <!-- End - comment -->

  <!-- Begin - agreement -->
  <?php print drupal_render($term_agreement); ?>
  <!-- End - agreement -->

  <!-- Begin - submit -->
  <?php print drupal_render($submit); ?>
  <!-- End - submit -->

</div>
