import 'package:dio/dio.dart';
import 'package:shms/model/env.dart';

import 'model/water.dart';
/* import 'package:shms/model/water.dart'; */

class WaterModelWrapper {
  int total;
  List<WaterModel> data;

  WaterModelWrapper(this.total, this.data);
}

class EnvModelWrapper {
  double aTemp;
  double aHum;
  List<EnvModel> data;

  EnvModelWrapper(this.aTemp, this.aHum, this.data);
}

Future<WaterModelWrapper> getWaterData(
    String year, String month, String day) async {
  try {
    var response = await Dio().get(
        'https://shms.buatkode.com/water?year=$year&month=$month&day=$day');
    if (response.statusCode == 200) {
      List<WaterModel> data = [];
      int total = 0;
      for (var element in (response.data as List<dynamic>)) {
        var result = WaterModel.fromJson(element);
        data.add(result);
        total += result.a;
      }
      return WaterModelWrapper(total, data);
    } else {
      throw Exception("error");
    }
  } catch (e) {
    throw Exception("Error");
  }
}

Future<EnvModelWrapper> getEnvData(
    String year, String month, String day) async {
  try {
    var response = await Dio()
        .get('https://shms.buatkode.com/dht?year=$year&month=$month&day=$day');
    if (response.statusCode == 200) {
      List<EnvModel> data = [];
      int totalHumidity = 0;
      int totalTemp = 0;
      for (var element in (response.data as List<dynamic>)) {
        var result = EnvModel.fromJson(element);
        data.add(result);
        totalHumidity += result.h;
        totalTemp += result.t;
      }
      return EnvModelWrapper(
          totalTemp / data.length, totalHumidity / data.length, data);
    } else {
      throw Exception("error");
    }
  } catch (e) {
    throw Exception("Error");
  }
}
