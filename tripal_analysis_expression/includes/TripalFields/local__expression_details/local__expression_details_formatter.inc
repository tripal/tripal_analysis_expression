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
    $header = ['Keyword', 'Expression Relationship', 'P-value', 'Analysis', 'Evidence Code'];
    $field = str_replace('_formatter', '', get_class());
    $variables = $entity->{$field}['und'][0];
    $pvalue_data = $variables['value'];
    $rows = [];
    #dpm($entity->chado_record);
    $organism_id = $entity->chado_record->organism_id->organism_id;
    foreach ($pvalue_data as $row) {
      $rows[] = [ # features that have the cvterm and are related to that organism
        l($row->cvterm_name, 'tripal_analysis_expression/pvalue/cvterm/' . $row->cvterm_id . '/' . $organism_id),
        $row->evidence_code,
        $row->p_value,
        l($row->analysis_name, 'bio_data/' . chado_get_record_entity_by_table('analysis', $row->analysis_id)),
        l('RNA-sequencing evidence used in manual assertion', '/cv/lookup/ECO/0006068'),
      ];
    }
    $element[0] = [
      '#type' => 'markup',
      '#markup' => theme('table', [
        'header' => $header,
        'rows' => $rows,
      ]),
    ];
    /*
      // adding second table onto linked page (Sig. Expression -> Keyword -> link)
      $settings = $display['settings'];
      $feature_names = $items[1]['value'];
      $element[1] =
        [
          "type" => "markup",
          "#markup" => theme('table', [
            "header" => ['organism', 'cvterm'],
            "rows" => $feature_names
          ]).$items[1]['pager']
        ];
    */
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

