<?php
/**
 * @file
 * Contains functions displaying administrative pages and forms.
 *
 * This file contains the functions used for the administration
 * of the arraydesign content type.
 *
 * @ingroup tripal_arraydesign
 */

/**
 * Administration page callbacks for the arraydesign content type.
 *
 * @return array
 *   A form API array describing an administrative form.
 *
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_admin() {

  $form = array();

  $details = array(
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_arraydesign',
    'options' => array(
      '[arraydesign.name]' => 'Array Design Only',
    ),
    'unique_option' => '[arraydesign.name]',
  );
  chado_add_admin_form_set_title($form, $form_state, $details);

  $details = array(
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_arraydesign',
  // *** fix these.
    'options' => array(
      '/arraydesign/[arraydesign.arraydesign_id]' => 'Array Design ID',
      '/arraydesign/[arraydesign.name]' => 'Array Design Name',
    ),
  );
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}

/**
 * Landing page for administration.
 *
 * Ensures Views are enabled and if not, provides links to do so.
 *
 * @return string
 *   An HTML string that provides a link to enable the view.
 *
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_admin_arraydesign_view() {

  $output = '';

  // Set the breadcrumbs.
  // *** update this for tripal.
  $breadcrumb = array();
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Administration'), 'admin');
  $breadcrumb[] = l(t('Tripal'), 'admin/tripal');
  $breadcrumb[] = l(t('Extensions'), 'admin/tripal/extension');
  $breadcrumb[] = l(t('Tripal Expression'), 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l(t('Analysis: Expression Content Types'), 'admin/tripal/extension/tripal_analysis_expression/content_types');
  $breadcrumb[] = l(t('Array Design'), 'admin/tripal/extension/tripal_analysis_expression/content_types/arraydesign');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view.
  $view = views_embed_view('tripal_arraydesign_admin_arraydesigns', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Expression module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this this '
      . 'administrative interface are disabled. <br><strong>Click each of the following links to '
    // *** fix this wording.
      . 'enable the pertinent views</strong>:</b>';
    $output .= '<ul>';
    $output .= '<li>' . l(t('Arraydesign View'), 'admin/tripal/extension/tripal_analysis_expression/content_type/arraydesign/views/arraydesigns/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}
