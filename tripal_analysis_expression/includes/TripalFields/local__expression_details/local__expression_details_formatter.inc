<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_details_formatter extends ChadoFieldFormatter
{
  // The default label for this field.
  public static $default_label = 'Pvalue Annotation';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__expression_details'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state)
  {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
  {
    $header = ['Keyword', 'Expression Relationship', 'P-value', 'Analysis'];
    $field = str_replace('_formatter', '', get_class());
    $variables = $entity->{$field}['und'][0];
    $pvalue_data = $variables['value'];
    $rows = [];

    foreach ($pvalue_data as $row) {
      $rows[] = [
        l($row->cvterm_name,'tripal_analysis_expression/pvalue/cvterm/'.$row->cvterm_id),#$cv_term->db_name,'/cv/lookup/'.$cv_term->db_name),
        #l($row->cvterm_name,'admin/tripal/extension/tripal_analysis_expression/cvterm/%'),
        $row->evidence_code,
        $row->p_value,
        l($row->analysis_name, 'bio_data/'. chado_get_record_entity_by_table('analysis', $row->analysis_id)),
      ];
    }

    $element[] = [
      '#type' => 'markup',
      '#markup' => theme('table', [
        'header' => $header,
        'rows' => $rows,
      ]),
    ];
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode)
  {
    return '';
  }
}
