target_sources(assimp
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/clipper/clipper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ConvertUTF/ConvertUTF.c
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/codegear/gtest_all.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/codegear/gtest_link.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample1.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample10_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample1_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample2.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample2_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample3_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample4.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample4_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample5_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample6_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample7_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample8_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample9_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-all.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-death-test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-filepath.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-port.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-printers.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-test-part.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-typed-test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest_main.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-death-test_ex_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-death-test_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-filepath_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-linked_ptr_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-listener_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-message_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-options_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-param-test2_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-param-test_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-port_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-printers_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-test-part_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-tuple_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-typed-test2_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-typed-test_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-unittest-api_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_all_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_break_on_failure_unittest_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_catch_exceptions_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_color_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_environment_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_env_var_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_filter_unittest_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_help_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_list_tests_unittest_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_main_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_no_test_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_output_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_pred_impl_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_premature_exit_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_prod_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_repeat_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_shuffle_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_sole_header_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_stress_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_throw_on_failure_ex_test.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_throw_on_failure_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_uninitialized_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_unittest.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_xml_outfile1_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_xml_outfile2_test_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest_xml_output_unittest_.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/production.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/xcode/Samples/FrameworkSample/widget.cc
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/xcode/Samples/FrameworkSample/widget_test.cc
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/irrXML.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcArithmeticCodec.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcDynamicVectorDecoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcDynamicVectorEncoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTriangleFans.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/code/DDLNode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/code/OpenDDLCommon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/code/OpenDDLExport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/code/OpenDDLParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/code/Value.cpp
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/common/shapes.cc
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/advancing_front.cc
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/cdt.cc
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/sweep.cc
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/sweep_context.cc
    ${CMAKE_CURRENT_LIST_DIR}/unzip/ioapi.c
    ${CMAKE_CURRENT_LIST_DIR}/unzip/unzip.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/adler32.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/compress.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/crc32.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/deflate.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/gzclose.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/gzlib.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/gzread.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/gzwrite.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/infback.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inffast.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inflate.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inftrees.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/trees.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/uncompr.c
    ${CMAKE_CURRENT_LIST_DIR}/zlib/zutil.c
#  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/clipper/clipper.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ConvertUTF/ConvertUTF.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-death-test.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-message.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-param-test.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-printers.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-spi.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-test-part.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest-typed-test.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest_pred_impl.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/gtest_prod.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-death-test-internal.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-filepath.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-internal.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-linked_ptr.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-param-util-generated.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-param-util.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-port-arch.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-port.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-string.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-tuple.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/gtest-type-util.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/custom/gtest-port.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/custom/gtest-printers.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/include/gtest/internal/custom/gtest.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/prime_tables.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample1.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample2.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample3-inl.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/samples/sample4.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/src/gtest-internal-inl.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-param-test_test.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/gtest-typed-test_test.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/test/production.h
#    ${CMAKE_CURRENT_LIST_DIR}/gtest/xcode/Samples/FrameworkSample/widget.h
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/CXMLReaderImpl.h
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/heapsort.h
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/irrArray.h
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/irrString.h
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/irrTypes.h
    ${CMAKE_CURRENT_LIST_DIR}/irrXML/irrXML.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcAdjacencyInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcArithmeticCodec.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcBinaryStream.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcCommon.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcDVEncodeParams.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcDynamicVector.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcDynamicVectorDecoder.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcDynamicVectorEncoder.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcFIFO.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcIndexedFaceSet.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcIndexedFaceSet.inl
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcSC3DMCDecoder.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcSC3DMCDecoder.inl
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcSC3DMCEncodeParams.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcSC3DMCEncoder.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcSC3DMCEncoder.inl
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTimer.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTriangleFans.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTriangleListDecoder.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTriangleListDecoder.inl
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTriangleListEncoder.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcTriangleListEncoder.inl
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcVector.h
    ${CMAKE_CURRENT_LIST_DIR}/Open3DGC/o3dgcVector.inl
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/include/openddlparser/DDLNode.h
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/include/openddlparser/OpenDDLCommon.h
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/include/openddlparser/OpenDDLExport.h
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/include/openddlparser/OpenDDLParser.h
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/include/openddlparser/OpenDDLParserUtils.h
    ${CMAKE_CURRENT_LIST_DIR}/openddlparser/include/openddlparser/Value.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/poly2tri.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/common/shapes.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/common/utils.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/advancing_front.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/cdt.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/sweep.h
    ${CMAKE_CURRENT_LIST_DIR}/poly2tri/poly2tri/sweep/sweep_context.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/allocators.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/document.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/encodedstream.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/encodings.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/filereadstream.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/filewritestream.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/memorybuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/memorystream.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/pointer.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/prettywriter.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/rapidjson.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/reader.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/stringbuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/writer.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/error/en.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/error/error.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/biginteger.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/diyfp.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/dtoa.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/ieee754.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/itoa.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/meta.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/pow10.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/stack.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/strfunc.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/strtod.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/internal/swap.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/msinttypes/inttypes.h
    ${CMAKE_CURRENT_LIST_DIR}/rapidjson/include/rapidjson/msinttypes/stdint.h
    ${CMAKE_CURRENT_LIST_DIR}/unzip/crypt.h
    ${CMAKE_CURRENT_LIST_DIR}/unzip/ioapi.h
    ${CMAKE_CURRENT_LIST_DIR}/unzip/unzip.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/crc32.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/deflate.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/gzguts.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inffast.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inffixed.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inflate.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/inftrees.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/trees.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/zconf.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/zconf.in.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/zlib.h
    ${CMAKE_CURRENT_LIST_DIR}/zlib/zutil.h
)
