<?php
/**
 * @file
 * Contains functions displaying administrative pages and forms.
 *
 * @ingroup tripal_analysis_expression
 */

/**
 * Administration page callbacks for the Tripal Analysis: Expression module.
 *
 * @return array
 *   A form API array describing an administrative form.
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_admin() {

  $form = [];

  $details = [
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_analysis_expression',
    'options' => [
      '[analysis_expression.name]' => 'Analysis: Expression Name Only',
    ],
    // *** fix  me.
    'unique_option' => '[analysis_expression.name]',
  ];
  chado_add_admin_form_set_title($form, $form_state, $details);

  $details = [
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_analysis_expression',
    // *** fix these.
    'options' => [
      '/analysis_expression/[analysis_expression.analysis_expression_id]' => 'Analysis: Expression ID (Analysis ID)',
      '/analysis_expression/[analysis_expression.name]' => 'Analysis: Expression Name',
    ],
  ];
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
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_admin_analysis_expression_view() {

  $output = '';

  // Set the breadcrumbs.
  $breadcrumb = [];
  $breadcrumb[] = l(t('Home'), '<front>');
  $breadcrumb[] = l(t('Administration'), 'admin');
  $breadcrumb[] = l(t('Tripal'), 'admin/tripal');
  $breadcrumb[] = l(t('Extensions'), 'admin/tripal/extension');
  $breadcrumb[] = l(t('Tripal Expression'), 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l(t('Analysis: Expression Content Types'), 'admin/tripal/extension/tripal_analysis_expression/content_types');
  $breadcrumb[] = l(t('Analysis: Expressions'), 'admin/tripal/extension/tripal_analysis_expression/content_types/analysis_expression');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view.
  $view = views_embed_view('tripal_analysis_expression_admin_analysis_expressions', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Expression module uses primarily views to provide an ' . 'administrative interface. Currently one or more views needed for this this ' . 'administrative interface are disabled. <br><strong>Click each of the following links to ' . 'enable the pertinent views</strong>:</b>';
    $output .= '<ul>';
    $output .= '<li>' . l(t('Analysis: Expression View'), 'admin/tripal/extension/tripal_analysis_expression/content_types/analysis_expression/views/analysis_expressions/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}
