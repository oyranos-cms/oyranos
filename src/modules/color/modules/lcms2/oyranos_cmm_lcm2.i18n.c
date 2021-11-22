#define oyranos_json "{\
  \"org\": {\
    \"freedesktop\": {\
      \"oyjl\": {\
        \"comment\": \"This is the Oyjl namespace\",\
        \"modules\": [{\
            \"comment\": \"This is the Oyjl-Module-API namespace\",\
            \"comment\": \"Internationalisation: Please translate only the following keys label, name, description, help. The translated strings shall be add as new keys at the same paths level by appending .lang_country identifier. e.g. org/freedesktop/oyjl/modules/[0]/name=ExampleCMM => org/freedesktop/oyjl/modules/[0]/name.de=BeispielCMM\",\
            \"oyjl_module_api_version\": \"1\",\
            \"comment\": \"We describe here a particular CMM. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *oyjl_modules_author*, *documentation* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier.\",\
            \"type\": \"CMM\",\
            \"label\": \"CMM\",\
            \"nick\": \"lcm2\",\
            \"name\": \"Little CMS 2\",\
            \"description\": \"Little Color Management System\",\
            \"information\": [{\
                \"type\": \"manufacturer\",\
                \"label\": \"Manufacturer\",\
                \"name\": \"Marti Maria\",\
                \"nick\": \"mm2\",\
                \"description\": \"http://www.littlecms.com\"\
              },{\
                \"type\": \"copyright\",\
                \"label\": \"Copyright\",\
                \"name\": \"Copyright 2018 Marti Maria\"\
              },{\
                \"type\": \"license\",\
                \"label\": \"License\",\
                \"name\": \"MIT\",\
                \"description\": \"http://www.littlecms.com\"\
              },{\
                \"type\": \"url\",\
                \"label\": \"URL\",\
                \"name\": \"http://www.littlecms.com\"\
              },{\
                \"type\": \"support\",\
                \"label\": \"Support\",\
                \"name\": \"http://www.littlecms.com/\"\
              },{\
                \"type\": \"download\",\
                \"label\": \"Download\",\
                \"name\": \"http://www.littlecms.com/\"\
              },{\
                \"type\": \"sources\",\
                \"label\": \"Sources\",\
                \"name\": \"http://www.littlecms.com/\"\
              },{\
                \"type\": \"development\",\
                \"label\": \"Development\",\
                \"name\": \"https://github.com/mm2/little-cms\"\
              },{\
                \"type\": \"oyjl_module_author\",\
                \"label\": \"Oyjl Module Author\",\
                \"name\": \"Kai-Uwe Behrmann\",\
                \"description\": \"http://www.behrmann.name\"\
              },{\
                \"type\": \"documentation\",\
                \"label\": \"Documentation\",\
                \"name\": \"http://www.openicc.info\",\
                \"description\": \"The module expects options for profiles and rendering_intent. Optional are rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\"\
              }],\
            \"logo\": \"lcms_logo2\",\
            \"groups\": [{\
                \"comment\": \"Logical group\",\
                \"name\": \"Little CMS 2\",\
                \"properties\": \"h3.frame\",\
                \"groups\": [{\
                    \"comment\": \"Logical sub group for presentation. *name* might be shown as label alone or together with *description*.\",\
                    \"name\": \"lcm2 Specific Options\",\
                    \"description\": \"Set lcm2 Options\",\
                    \"help\": \"CMM options for color rendering, precission and fixes.\",\
                    \"properties\": \"h4\",\
                    \"options\": [{\
                        \"key\": \"org/freedesktop/openicc/icc_color/cmyk_cmyk_black_preservation\",\
                        \"default\": \"0\",\
                        \"properties\": \"rendering\",\
                        \"type\": \"choice\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"[none]\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"LCMS_PRESERVE_PURE_K\"\
                          },{\
                            \"nick\": \"2\",\
                            \"name\": \"LCMS_PRESERVE_K_PLANE\"\
                          }],\
                        \"name\": \"Black Preservation\",\
                        \"description\": \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\",\
                        \"help\": \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS 2 has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\"\
                      },{\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation\",\
                        \"default\": \"0\",\
                        \"properties\": \"rendering.advanced\",\
                        \"type\": \"choice\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"normal\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"LCMS2_NOOPTIMIZE\"\
                          },{\
                            \"nick\": \"2\",\
                            \"name\": \"LCMS2_HIGHRESPRECALC\"\
                          },{\
                            \"nick\": \"3\",\
                            \"name\": \"LCMS2_LOWRESPRECALC\"\
                          },{\
                            \"nick\": \"4\",\
                            \"name\": \"LCMS2_NOTRANSFORM\"\
                          }],\
                        \"name\": \"Optimization\",\
                        \"description\": \"Color Transforms can be differently stored internally\",\
                        \"help\": \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\"\
                      },{\
                        \"key\": \"org/freedesktop/openicc/icc_color/precalculation_curves\",\
                        \"default\": \"1\",\
                        \"properties\": \"rendering.advanced.boolean\",\
                        \"type\": \"choice\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"[none]\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"LCMS2_POST+PRE_CURVES\"\
                          }],\
                        \"name\": \"Curves for Optimization\",\
                        \"description\": \"Color Transform CLUT's can additionally use curves for special cases\",\
                        \"help\": \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\"\
                      },{\
                        \"key\": \"org/freedesktop/openicc/icc_color/adaption_state\",\
                        \"default\": \"1\",\
                        \"properties\": \"rendering.advanced\",\
                        \"type\": \"choice\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"0.0\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"1.0\"\
                          }],\
                        \"name\": \"Adaptation State\",\
                        \"description\": \"Adaptation state for absolute colorimetric intent\",\
                        \"help\": \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\"\
                      },{\
                        \"key\": \"org/freedesktop/openicc/icc_color/no_white_on_white_fixup\",\
                        \"default\": \"1\",\
                        \"properties\": \"rendering.advanced\",\
                        \"type\": \"bool\",\
                        \"choices\": [{\
                            \"nick\": \"0\",\
                            \"name\": \"No\"\
                          },{\
                            \"nick\": \"1\",\
                            \"name\": \"Yes\"\
                          }],\
                        \"name\": \"No White on White Fix\",\
                        \"description\": \"Skip White Point on White point alignment\",\
                        \"help\": \"Avoid force of White on White mapping. Default for absolute rendering intent.\"\
                      }]\
                  }]\
              }]\
          }],\
        \"translations\": {\
          \"cs_CZ\": {\
            \"CMM\": \"CMM\",\
            \"Little CMS 2\": \"Little CMS 2\",\
            \"Manufacturer\": \"Výrobce\",\
            \"Marti Maria\": \"Marti Maria\",\
            \"Copyright\": \"Copyright\",\
            \"License\": \"Licence\",\
            \"MIT\": \"MIT\",\
            \"Kai-Uwe Behrmann\": \"Kai-Uwe Behrmann\",\
            \"Documentation\": \"Dokumentace\",\
            \"[none]\": \"[žádný]\",\
            \"Black Preservation\": \"Zachování černé\",\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Rozhodnout, jakým způsobem zachovat černý kanál při transformaci CMYK -> CMYK.\",\
            \"Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS 2 has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane.\": \"Při transformaci CMYK -> CMYK lze použít různé strategie pro zachování černého kanálu. 'Žádný' znamená, že černá se může změnit na CMY a text se tedy nevytiskne nejlépe. LittleCMS 2 to řeší dvěma způsoby: zachování pouze černého inkoustu nebo zachování roviny černé. První způsob je jednoduchý a účinný: provedou se všechny kolorimetrické transformace, ale zachová se jen K (světlost L*) v případech, kdy vstupní obrázek je pouze černý. Druhý způsob je značně komplexní a snaží se zachovat celou rovinu černé (K).\",\
            \"Optimization\": \"Optimalizace\",\
            \"Color Transforms can be differently stored internally\": \"Transformace barev mohou být interně uloženy různými způsoby\",\
            \"Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags.\": \"Little CMS se snaží optimalizovat řetězení profilů kdykoliv je to možné. Některá optimalizační schémata jsou již přednastavena, nová schémata můžete přidat skrze plug-in. Obvykle dojde ke zlepšení výkonu transformace, ale může dojít ke zpoždění při vytváření metody transformace. Pokud budete transformovat pouze několik barev, pak nepotřebujete tyto předběžné výpočty. V takovém případě lze použít příznak cmsFLAGS_NOOPTIMIZE v cmsCreateTransform() k zabránění procesu optimalizace. Viz reference API pro detailnější diskuzi ohledně příznaků.\",\
            \"Curves for Optimization\": \"Křivky pro optimalizaci\",\
            \"Color Transform CLUT's can additionally use curves for special cases\": \"Transformace barev typu CLUT může ve zvláštních případech využít i křivky.\",\
            \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\": \"Ve zvláštních případech jako hodnoty kódované v gama do a z lineárních hodnot může Little CMS využít křivky před a po CLUT. Výkon však poklesne.\",\
            \"Adaptation State\": \"Stav adaptace\",\
            \"Adaptation state for absolute colorimetric intent\": \"Adaptační stav pro absolutní kolorimetrický záměr\",\
            \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\": \"Adaptační stav by měl být mezi 0 a 1.0 a týká se absolutního kolorimetrického záměru.\",\
            \"No\": \"Ne\",\
            \"Yes\": \"Ano\"\
          },\
          \"de_DE\": {\
            \"CMM\": \"CMM\",\
            \"Little CMS 2\": \"Little CMS 2\",\
            \"Little Color Management System\": \"Little Color Management System\",\
            \"Manufacturer\": \"Hersteller\",\
            \"Marti Maria\": \"Marti Maria\",\
            \"http://www.littlecms.com\": \"http://www.littlecms.com\",\
            \"Copyright\": \"Kopierrecht\",\
            \"Copyright 2018 Marti Maria\": \"Kopierrecht 2018 Marti Maria\",\
            \"License\": \"Lizenz\",\
            \"MIT\": \"MIT\",\
            \"http://www.littlecms.com\": \"http://www.littlecms.com\",\
            \"URL\": \"URL\",\
            \"http://www.littlecms.com\": \"http://www.littlecms.com\",\
            \"Support\": \"Unterstützung\",\
            \"http://www.littlecms.com/\": \"http://www.littlecms.com\",\
            \"Download\": \"Bezugsquelle\",\
            \"http://www.littlecms.com/\": \"http://www.littlecms.com\",\
            \"Sources\": \"Quellen\",\
            \"http://www.littlecms.com/\": \"http://www.littlecms.com\",\
            \"Development\": \"Entwicklung\",\
            \"https://github.com/mm2/little-cms\": \"https://github.com/mm2/little-cms\",\
            \"Oyjl Module Author\": \"Oyjl Modul Autor\",\
            \"Kai-Uwe Behrmann\": \"Kai-Uwe Behrmann\",\
            \"http://www.behrmann.name\": \"http://www.behrmann.name\",\
            \"Documentation\": \"Dokumente\",\
            \"http://www.openicc.info\": \"http://www.openicc.info\",\
            \"The module expects options for profiles and rendering_intent. Optional are rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\": \"Das Modul erwartet Optionen für Profile und Übertragungsfunktion. Optional sind rendering_bpc, cmyk_cmyk_black_preservation, precalculation_curves, adaption_state, no_white_on_white_fixup.\",\
            \"lcm2 Specific Options\": \"lcm2 Spezifische Optionen\",\
            \"Set lcm2 Options\": \"Editiere lcm2 Optionen\",\
            \"CMM options for color rendering, precission and fixes.\": \"CMM Optionen für Farbumwandlung, Präzission und Verbesserungen.\",\
            \"[none]\": \"[kein]\",\
            \"LCMS_PRESERVE_PURE_K\": \"LCMS_PRESERVE_PURE_K\",\
            \"LCMS_PRESERVE_K_PLANE\": \"LCMS_PRESERVE_K_PLANE\",\
            \"Black Preservation\": \"Schwarzerhalt\",\
            \"Decide how to preserve the black channel for Cmyk to Cmyk transforms\": \"Entscheide wie der Schwarzkanal erhalten wird bei Umwandlungen von einem Druckfarbraum in einen Anderen\",\
            \"normal\": \"normal\",\
            \"LCMS2_NOOPTIMIZE\": \"LCMS2_NOOPTIMIZE\",\
            \"LCMS2_HIGHRESPRECALC\": \"LCMS2_HIGHRESPRECALC\",\
            \"LCMS2_LOWRESPRECALC\": \"LCMS2_LOWRESPRECALC\",\
            \"LCMS2_NOTRANSFORM\": \"LCMS2_NOTRANSFORM\",\
            \"Optimization\": \"Optimierung\",\
            \"Color Transforms can be differently stored internally\": \"Farbumwandlung können intern unterschiedlich gespeichert werden\",\
            \"LCMS2_POST+PRE_CURVES\": \"LCMS2_POST+PRE_CURVES\",\
            \"Curves for Optimization\": \"Kurven für Optimierungen\",\
            \"Color Transform CLUT's can additionally use curves for special cases\": \"Farbtransformationstabellen können für spezielle Fälle zusätzlich mit Kurven kombiniert werden\",\
            \"Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer.\": \"Little CMS kann Kurven vor und nach 3D Interpolationstabellen benutzen. Dadurch zeigen sich weniger Fehler bei der Übertragung zwischen linearen und gammabehafteten Werten auf Kosten der Geschwindigkeit.\",\
            \"0.0\": \"0,0\",\
            \"1.0\": \"1,0\",\
            \"Adaptation State\": \"Farbanpassungsgrad\",\
            \"Adaptation state for absolute colorimetric intent\": \"Farbanpassungsgrad für die absolut farbmetrische Übertragung\",\
            \"The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent.\": \"Der Farbanpassungsgrad sollte zwischen 0 und 1.0 liegen und wird auf die absolut farbmetrische Übertragung angewendet.\",\
            \"No\": \"Nein\",\
            \"Yes\": \"Ja\",\
            \"No White on White Fix\": \"Keine Weiß zu Weiß Reparatur\",\
            \"Skip White Point on White point alignment\": \"Lasse Weiß auf Weiß Einpassung weg\",\
            \"Avoid force of White on White mapping. Default for absolute rendering intent.\": \"Lasse das Erzwingen einer Weiß zu Weiß übertragung fort. Standard bei Absolut Farbmetrischer Übertragung.\"\
          },\
          \"eo\": {\
            \"Yes\": \"Jes\"\
          },\
          \"fr_FR\": {\
            \"Copyright\": \"Copyright\"\
          }\
        }\
      }\
    }\
  }\
}"
