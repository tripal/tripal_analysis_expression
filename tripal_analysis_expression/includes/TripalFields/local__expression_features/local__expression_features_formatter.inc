<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_features_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Expression Features';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__expression_features');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

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
    $pvalue_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'OBI'],
      'name' => 'p-value',
    ])->cvterm_id;
    $evidence_code_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'local'],
      'name' => 'evidence code',
    ])->cvterm_id;
    $header = [
      ['data' => 'Feature Name', 'field' => 'f.name', 'sort' => 'ASC'],
      ['data' => 'Keyword', 'field' => 'cv.name', 'sort' => 'ASC'],
      ['data' => 'Relationship', 'field' => 'fp2.value', 'sort' => 'ASC'],
      ['data' => 'P-value', 'field' => 'fp.value', 'sort' => 'ASC'],
    ];
    $rows = [];

    $query = chado_db_select('analysis', 'a')
      ->extend('TableSort');
    $analysis_id = $entity->chado_record->analysis_id;
    $query->fields('f', array('name')); //feature name and id for link
    $query->fields('fp', array('value')); // p-values (shares 'value' column in the table feature_cvtermprop)
    $query->fields('fp2', array('value')); // expression relationship (shares 'value' column in the table feature_cvtermprop)
    $query->fields('cv', array('name')); // Keyword
    $query->join('analysisfeature', 'af', 'a.analysis_id = af.analysis_id');
    $query->join('feature', 'f', 'f.feature_id = af.feature_id');
    $query->join('feature_cvterm', 'fc', 'f.feature_id = fc.feature_id');
    $query->join('feature_cvtermprop', 'fp', 'fc.feature_cvterm_id = fp.feature_cvterm_id AND fp.type_id = :typeid', [':typeid' => $pvalue_cvterm_id]);
    $query->join('feature_cvtermprop', 'fp2', 'fp.feature_cvterm_id = fp2.feature_cvterm_id AND fp2.type_id = :typeid2', [':typeid2' => $evidence_code_cvterm_id]);
    $query->join('cvterm', 'cv', 'fc.cvterm_id = cv.cvterm_id');
    $query->condition('a.analysis_id', $analysis_id, '=');
    $query->orderByHeader($header);
    $total = $query->countQuery()->execute()->fetchField();
    $page = pager_default_initialize($total, 10);
    $query->range($page * 10, 10);
    $result = $query->execute();

    while ($record = $result->fetchAssoc()) {
      $rows[] = [$record['name'], $record['cv_name'], $record['fp2_value'], $record['value']];
    }

    $pager = theme('pager', ['quantity', $total]);
    $pager = $this->ajaxifyPager($pager, $entity);

    $table = theme('table',
      [
        "header" => $header,
        "rows" => $rows,
        'sort' => 'ASC',
        'pager' => $pager,
      ]);

    $element[0] =
      [
        "#type" => "markup",
        "#markup" => $table . $pager,
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
