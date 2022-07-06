; ModuleID = 'src//AmbisonicCommons.cpp'
source_filename = "src//AmbisonicCommons.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@switch.table._Z24OrderToComponentPositionjb = private unnamed_addr constant [3 x i32] [i32 1, i32 4, i32 10], align 4
@switch.table._Z23ComponentToChannelLabeljb = private unnamed_addr constant [16 x i8] c"WYZXVTRUSQOMKLNP", align 1

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local float @_Z16DegreesToRadiansf(float %fDegrees) local_unnamed_addr #0 {
entry:
  %div = fmul fast float %fDegrees, 0x3F91DF46C0000000
  ret float %div
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local float @_Z16RadiansToDegreesf(float %fRadians) local_unnamed_addr #0 {
entry:
  %div = fmul fast float %fRadians, 0x404CA5DC00000000
  ret float %div
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local i32 @_Z17OrderToComponentsjb(i32 %nOrder, i1 zeroext %b3D) local_unnamed_addr #0 {
entry:
  br i1 %b3D, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %conv = uitofp i32 %nOrder to float
  %add = fadd fast float %conv, 1.000000e+00
  %square = fmul fast float %add, %add
  %conv1 = fptoui float %square to i32
  br label %return

if.else:                                          ; preds = %entry
  %mul = shl i32 %nOrder, 1
  %add2 = or i32 %mul, 1
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ %conv1, %if.then ], [ %add2, %if.else ]
  ret i32 %retval.0
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local i32 @_Z24OrderToComponentPositionjb(i32 %nOrder, i1 zeroext %b3D) local_unnamed_addr #0 {
entry:
  %switch.tableidx = add i32 %nOrder, -1
  %0 = icmp ult i32 %switch.tableidx, 3
  br i1 %b3D, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br i1 %0, label %switch.lookup, label %if.end

if.else:                                          ; preds = %entry
  br i1 %0, label %switch.lookup10, label %if.end

switch.lookup:                                    ; preds = %if.then
  %1 = sext i32 %switch.tableidx to i64
  %switch.gep = getelementptr inbounds [3 x i32], [3 x i32]* @switch.table._Z24OrderToComponentPositionjb, i64 0, i64 %1
  %switch.load = load i32, i32* %switch.gep, align 4
  ret i32 %switch.load

switch.lookup10:                                  ; preds = %if.else
  %switch.idx.mult = shl i32 %switch.tableidx, 1
  %switch.offset = or i32 %switch.idx.mult, 1
  ret i32 %switch.offset

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local i32 @_Z15OrderToSpeakersjb(i32 %nOrder, i1 zeroext %b3D) local_unnamed_addr #0 {
entry:
  %mul = shl i32 %nOrder, 1
  %add = add i32 %mul, 2
  %mul1 = zext i1 %b3D to i32
  %retval.0 = shl i32 %add, %mul1
  ret i32 %retval.0
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local signext i8 @_Z23ComponentToChannelLabeljb(i32 %nComponent, i1 zeroext %b3D) local_unnamed_addr #0 {
entry:
  br i1 %b3D, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %0 = icmp ult i32 %nComponent, 16
  br i1 %0, label %switch.lookup, label %if.end

if.else:                                          ; preds = %entry
  %1 = icmp ult i32 %nComponent, 7
  br i1 %1, label %switch.lookup24, label %if.end

switch.lookup:                                    ; preds = %if.then
  %2 = sext i32 %nComponent to i64
  %switch.gep = getelementptr inbounds [16 x i8], [16 x i8]* @switch.table._Z23ComponentToChannelLabeljb, i64 0, i64 %2
  %switch.load = load i8, i8* %switch.gep, align 1
  ret i8 %switch.load

switch.lookup24:                                  ; preds = %if.else
  %switch.cast = zext i32 %nComponent to i56
  %switch.shiftamt = shl nuw nsw i56 %switch.cast, 3
  %switch.downshift = lshr i56 22887804842891351, %switch.shiftamt
  %switch.masked = trunc i56 %switch.downshift to i8
  ret i8 %switch.masked

if.end:                                           ; preds = %if.else, %if.then
  ret i8 32
}

attributes #0 = { norecurse nounwind readnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (https://gitlab.engr.illinois.edu/llvm/hpvm-release.git f83c355d11be7d011508763fedc67e76d592a636)"}
