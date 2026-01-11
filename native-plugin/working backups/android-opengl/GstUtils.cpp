#include "GstUtils.h"

namespace GstUtils {
    void print_negotiated_caps(GstElement *pipeline) {
        GstIterator *iter;
        GValue val = G_VALUE_INIT;

        iter = gst_bin_iterate_elements(GST_BIN(pipeline));
        while (gst_iterator_next(iter, &val) == GST_ITERATOR_OK) {
            GstElement *elem = GST_ELEMENT(g_value_get_object(&val));
            Logger::logFormat("%s", GST_ELEMENT_NAME(elem));

            GstIterator *pad_iter;
            GValue pad_val = G_VALUE_INIT;
            pad_iter = gst_element_iterate_pads(elem);

            while (gst_iterator_next(pad_iter, &pad_val) == GST_ITERATOR_OK) {
                GstPad *pad = GST_PAD(g_value_get_object(&pad_val));
                if (GST_PAD_DIRECTION(pad) == GST_PAD_SRC) {
                    GstCaps *caps = gst_pad_get_current_caps(pad);
                    if (caps) {
                        gchar *str = gst_caps_to_string(caps);
                        Logger::logFormat("Src pad: %s", GST_PAD_NAME(pad));
                        Logger::logFormat("Caps: %s", str);
                        g_free(str);
                        gst_caps_unref(caps);
                    } else {
                        Logger::logFormat("%s has no caps yet.", GST_PAD_NAME(pad));
                    }
                }
                g_value_reset(&pad_val);
            }

            gst_iterator_free(pad_iter);
            g_value_reset(&val);
        }

        gst_iterator_free(iter);
    }

    void print_context(GStreamerInstance *instance) {
        GstElement *sink = gst_bin_get_by_name(GST_BIN(instance->pipeline), "unity");

        GstGLContext *gst_ctx = NULL;
        g_object_get(sink, "context", &gst_ctx, NULL);

        if (!gst_ctx) {
            Logger::log("glimagesink has no context.");
            return;
        }

        EGLContext egl_ctx =
            (EGLContext)gst_gl_context_get_gl_context(gst_ctx);
        Logger::logFormat("glimagesink context: %p", egl_ctx);
    }

    void print_element_caps(GstElement* element) {
        GstIterator* it = gst_element_iterate_src_pads(element);
        GstIterator* it2 = gst_element_iterate_sink_pads(element);

        GValue value = G_VALUE_INIT;
        gboolean done = FALSE;

        Logger::log("---SRC---");
        while (!done) {
            switch (gst_iterator_next(it, &value)) {
                case GST_ITERATOR_OK: {
                    GstPad* pad = GST_PAD(g_value_get_object(&value));
                    GstCaps* caps = gst_pad_query_caps(pad, nullptr);

                    gchar* caps_str = gst_caps_to_string(caps);
                    Logger::logFormat("Src caps:\n%s\n", caps_str);

                    g_free(caps_str);
                    gst_caps_unref(caps);
                    g_value_unset(&value);
                    break;
                }
                case GST_ITERATOR_RESYNC:
                    gst_iterator_resync(it);
                    break;
                case GST_ITERATOR_DONE:
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }

        done = false;
        Logger::log("---SINK---");
        while (!done) {
            switch (gst_iterator_next(it2, &value)) {
                case GST_ITERATOR_OK: {
                    GstPad* pad = GST_PAD(g_value_get_object(&value));
                    GstCaps* caps = gst_pad_query_caps(pad, nullptr);

                    gchar* caps_str = gst_caps_to_string(caps);
                    Logger::logFormat("Sink caps:\n%s\n", caps_str);

                    g_free(caps_str);
                    gst_caps_unref(caps);
                    g_value_unset(&value);
                    break;
                }
                case GST_ITERATOR_RESYNC:
                    gst_iterator_resync(it2);
                    break;
                case GST_ITERATOR_DONE:
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }

        gst_iterator_free(it);
        gst_iterator_free(it2);
    }

    void print_decoders() {
        GList *decoders = gst_element_factory_list_get_elements(GST_ELEMENT_FACTORY_TYPE_DECODABLE,
                                                        GST_RANK_MARGINAL);
        for (GList *iter = decoders; iter != NULL; iter = iter->next) {
            GstElementFactory *factory = (GstElementFactory *) iter->data;

            // Get the factory name suitable for use in a string pipeline
            const gchar *name = gst_element_get_name(factory);

            // Print the factory name
            Logger::logFormat("Decoder %s", name);
        }
    }
}