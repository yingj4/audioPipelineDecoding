; ModuleID = 'src//mit_hrtf.cpp'
source_filename = "src//mit_hrtf.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%class.MIT_HRTF = type { %class.HRTF }
%class.HRTF = type { i32 (...)**, i32, i32 }

$_ZN8MIT_HRTFD0Ev = comdat any

$_ZN4HRTFD2Ev = comdat any

$_ZTS4HRTF = comdat any

$_ZTI4HRTF = comdat any

@_ZTV8MIT_HRTF = dso_local unnamed_addr constant { [5 x i8*] } { [5 x i8*] [i8* null, i8* bitcast ({ i8*, i8*, i8* }* @_ZTI8MIT_HRTF to i8*), i8* bitcast (void (%class.HRTF*)* @_ZN4HRTFD2Ev to i8*), i8* bitcast (void (%class.MIT_HRTF*)* @_ZN8MIT_HRTFD0Ev to i8*), i8* bitcast (i1 (%class.MIT_HRTF*, float, float, float**)* @_ZN8MIT_HRTF3getEffPPf to i8*)] }, align 8
@_ZTVN10__cxxabiv120__si_class_type_infoE = external dso_local global i8*
@_ZTS8MIT_HRTF = dso_local constant [10 x i8] c"8MIT_HRTF\00", align 1
@_ZTVN10__cxxabiv117__class_type_infoE = external dso_local global i8*
@_ZTS4HRTF = linkonce_odr dso_local constant [6 x i8] c"4HRTF\00", comdat, align 1
@_ZTI4HRTF = linkonce_odr dso_local constant { i8*, i8* } { i8* bitcast (i8** getelementptr inbounds (i8*, i8** @_ZTVN10__cxxabiv117__class_type_infoE, i64 2) to i8*), i8* getelementptr inbounds ([6 x i8], [6 x i8]* @_ZTS4HRTF, i32 0, i32 0) }, comdat, align 8
@_ZTI8MIT_HRTF = dso_local constant { i8*, i8*, i8* } { i8* bitcast (i8** getelementptr inbounds (i8*, i8** @_ZTVN10__cxxabiv120__si_class_type_infoE, i64 2) to i8*), i8* getelementptr inbounds ([10 x i8], [10 x i8]* @_ZTS8MIT_HRTF, i32 0, i32 0), i8* bitcast ({ i8*, i8* }* @_ZTI4HRTF to i8*) }, align 8

@_ZN8MIT_HRTFC1Ej = dso_local unnamed_addr alias void (%class.MIT_HRTF*, i32), void (%class.MIT_HRTF*, i32)* @_ZN8MIT_HRTFC2Ej

; Function Attrs: uwtable
define dso_local void @_ZN8MIT_HRTFC2Ej(%class.MIT_HRTF* nocapture %this, i32 %i_sampleRate) unnamed_addr #0 align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %0 = getelementptr inbounds %class.MIT_HRTF, %class.MIT_HRTF* %this, i64 0, i32 0, i32 0
  %i_sampleRate2.i = getelementptr inbounds %class.MIT_HRTF, %class.MIT_HRTF* %this, i64 0, i32 0, i32 1
  store i32 %i_sampleRate, i32* %i_sampleRate2.i, align 8, !tbaa !2
  %i_len.i = getelementptr inbounds %class.MIT_HRTF, %class.MIT_HRTF* %this, i64 0, i32 0, i32 2
  store i32 0, i32* %i_len.i, align 4, !tbaa !7
  store i32 (...)** bitcast (i8** getelementptr inbounds ({ [5 x i8*] }, { [5 x i8*] }* @_ZTV8MIT_HRTF, i64 0, inrange i32 0, i64 2) to i32 (...)**), i32 (...)*** %0, align 8, !tbaa !8
  %call = tail call i32 @mit_hrtf_availability(i32 0, i32 0, i32 %i_sampleRate)
  store i32 %call, i32* %i_len.i, align 4, !tbaa !7
  ret void
}

declare dso_local i32 @mit_hrtf_availability(i32, i32, i32) local_unnamed_addr #1

declare dso_local i32 @__gxx_personality_v0(...)

; Function Attrs: uwtable
define dso_local zeroext i1 @_ZN8MIT_HRTF3getEffPPf(%class.MIT_HRTF* nocapture readonly %this, float %f_azimuth, float %f_elevation, float** nocapture readonly %pfHRTF) unnamed_addr #0 align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %nAzimuth = alloca i32, align 4
  %nElevation = alloca i32, align 4
  %0 = bitcast i32* %nAzimuth to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %0) #7
  %sub = fsub fast float -0.000000e+00, %f_azimuth
  %call = tail call fast float @_Z16RadiansToDegreesf(float %sub)
  %conv = fptosi float %call to i32
  %cmp = icmp sgt i32 %conv, 180
  %sub2 = add nsw i32 %conv, -360
  %spec.select = select i1 %cmp, i32 %sub2, i32 %conv
  store i32 %spec.select, i32* %nAzimuth, align 4, !tbaa !10
  %1 = bitcast i32* %nElevation to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %1) #7
  %call3 = tail call fast float @_Z16RadiansToDegreesf(float %f_elevation)
  %conv4 = fptosi float %call3 to i32
  store i32 %conv4, i32* %nElevation, align 4, !tbaa !10
  %i_len = getelementptr inbounds %class.MIT_HRTF, %class.MIT_HRTF* %this, i64 0, i32 0, i32 2
  %2 = load i32, i32* %i_len, align 4, !tbaa !7
  %conv5 = zext i32 %2 to i64
  %cmp.i.i.i.i = icmp eq i32 %2, 0
  br i1 %cmp.i.i.i.i, label %invoke.cont10, label %cond.true.i.i.i.i

cond.true.i.i.i.i:                                ; preds = %entry
  %mul.i.i.i.i.i.i = shl nuw nsw i64 %conv5, 1
  %call2.i.i.i.i3.i.i71 = invoke i8* @_Znwm(i64 %mul.i.i.i.i.i.i)
          to label %invoke.cont unwind label %ehcleanup.thread

invoke.cont:                                      ; preds = %cond.true.i.i.i.i
  %3 = bitcast i8* %call2.i.i.i.i3.i.i71 to i16*
  %4 = shl nuw nsw i64 %conv5, 1
  tail call void @llvm.memset.p0i8.i64(i8* nonnull align 2 %call2.i.i.i.i3.i.i71, i8 0, i64 %4, i1 false)
  %.pr = load i32, i32* %i_len, align 4, !tbaa !7
  %conv7 = zext i32 %.pr to i64
  %cmp.i.i.i.i76 = icmp eq i32 %.pr, 0
  br i1 %cmp.i.i.i.i76, label %invoke.cont10, label %cond.true.i.i.i.i77

cond.true.i.i.i.i77:                              ; preds = %invoke.cont
  %mul.i.i.i.i.i.i78 = shl nuw nsw i64 %conv7, 1
  %call2.i.i.i.i3.i.i88 = invoke i8* @_Znwm(i64 %mul.i.i.i.i.i.i78)
          to label %for.body.i.i.preheader.i.i.i.i.i85 unwind label %_ZNSt6vectorIsSaIsEED2Ev.exit95

for.body.i.i.preheader.i.i.i.i.i85:               ; preds = %cond.true.i.i.i.i77
  %5 = bitcast i8* %call2.i.i.i.i3.i.i88 to i16*
  %6 = shl nuw nsw i64 %conv7, 1
  tail call void @llvm.memset.p0i8.i64(i8* nonnull align 2 %call2.i.i.i.i3.i.i88, i8 0, i64 %6, i1 false)
  br label %invoke.cont10

invoke.cont10:                                    ; preds = %entry, %invoke.cont, %for.body.i.i.preheader.i.i.i.i.i85
  %7 = phi i16* [ %5, %for.body.i.i.preheader.i.i.i.i.i85 ], [ null, %invoke.cont ], [ null, %entry ]
  %8 = phi i8* [ %call2.i.i.i.i3.i.i88, %for.body.i.i.preheader.i.i.i.i.i85 ], [ null, %invoke.cont ], [ null, %entry ]
  %9 = phi i16* [ %3, %for.body.i.i.preheader.i.i.i.i.i85 ], [ %3, %invoke.cont ], [ null, %entry ]
  %10 = phi i8* [ %call2.i.i.i.i3.i.i71, %for.body.i.i.preheader.i.i.i.i.i85 ], [ %call2.i.i.i.i3.i.i71, %invoke.cont ], [ null, %entry ]
  %i_sampleRate = getelementptr inbounds %class.MIT_HRTF, %class.MIT_HRTF* %this, i64 0, i32 0, i32 1
  %11 = load i32, i32* %i_sampleRate, align 8, !tbaa !2
  %call19 = invoke i32 @mit_hrtf_get(i32* nonnull %nAzimuth, i32* nonnull %nElevation, i32 %11, i16* %9, i16* %7)
          to label %invoke.cont18 unwind label %lpad17

invoke.cont18:                                    ; preds = %invoke.cont10
  %cmp20 = icmp eq i32 %call19, 0
  br i1 %cmp20, label %cleanup, label %for.cond.preheader

for.cond.preheader:                               ; preds = %invoke.cont18
  %12 = load i32, i32* %i_len, align 4, !tbaa !7
  %cmp24104 = icmp eq i32 %12, 0
  br i1 %cmp24104, label %cleanup, label %for.body.lr.ph

for.body.lr.ph:                                   ; preds = %for.cond.preheader
  %13 = load float*, float** %pfHRTF, align 8, !tbaa !11
  %arrayidx38 = getelementptr inbounds float*, float** %pfHRTF, i64 1
  %14 = load float*, float** %arrayidx38, align 8, !tbaa !11
  %15 = zext i32 %12 to i64
  %xtraiter = and i64 %15, 1
  %16 = icmp eq i32 %12, 1
  br i1 %16, label %if.then.i.i.i74.loopexit.unr-lcssa, label %for.body.lr.ph.new

for.body.lr.ph.new:                               ; preds = %for.body.lr.ph
  %unroll_iter = sub nsw i64 %15, %xtraiter
  br label %for.body

ehcleanup.thread:                                 ; preds = %cond.true.i.i.i.i
  %17 = landingpad { i8*, i32 }
          cleanup
  %18 = extractvalue { i8*, i32 } %17, 0
  br label %ehcleanup56

_ZNSt6vectorIsSaIsEED2Ev.exit95:                  ; preds = %cond.true.i.i.i.i77
  %19 = landingpad { i8*, i32 }
          cleanup
  %20 = extractvalue { i8*, i32 } %19, 0
  call void @_ZdlPv(i8* nonnull %call2.i.i.i.i3.i.i71) #7
  br label %ehcleanup56

lpad17:                                           ; preds = %invoke.cont10
  %21 = landingpad { i8*, i32 }
          cleanup
  %22 = extractvalue { i8*, i32 } %21, 0
  %tobool.i.i.i = icmp eq i16* %7, null
  br i1 %tobool.i.i.i, label %_ZNSt6vectorIsSaIsEED2Ev.exit, label %if.then.i.i.i

for.body:                                         ; preds = %for.body, %for.body.lr.ph.new
  %indvars.iv = phi i64 [ 0, %for.body.lr.ph.new ], [ %indvars.iv.next.1, %for.body ]
  %niter = phi i64 [ %unroll_iter, %for.body.lr.ph.new ], [ %niter.nsub.1, %for.body ]
  %add.ptr.i91 = getelementptr inbounds i16, i16* %9, i64 %indvars.iv
  %23 = load i16, i16* %add.ptr.i91, align 2, !tbaa !13
  %conv29 = sitofp i16 %23 to float
  %div = fmul fast float %conv29, 0x3F00002000000000
  %arrayidx31 = getelementptr inbounds float, float* %13, i64 %indvars.iv
  store float %div, float* %arrayidx31, align 4, !tbaa !15
  %add.ptr.i = getelementptr inbounds i16, i16* %7, i64 %indvars.iv
  %24 = load i16, i16* %add.ptr.i, align 2, !tbaa !13
  %conv36 = sitofp i16 %24 to float
  %div37 = fmul fast float %conv36, 0x3F00002000000000
  %arrayidx40 = getelementptr inbounds float, float* %14, i64 %indvars.iv
  store float %div37, float* %arrayidx40, align 4, !tbaa !15
  %indvars.iv.next = or i64 %indvars.iv, 1
  %add.ptr.i91.1 = getelementptr inbounds i16, i16* %9, i64 %indvars.iv.next
  %25 = load i16, i16* %add.ptr.i91.1, align 2, !tbaa !13
  %conv29.1 = sitofp i16 %25 to float
  %div.1 = fmul fast float %conv29.1, 0x3F00002000000000
  %arrayidx31.1 = getelementptr inbounds float, float* %13, i64 %indvars.iv.next
  store float %div.1, float* %arrayidx31.1, align 4, !tbaa !15
  %add.ptr.i.1 = getelementptr inbounds i16, i16* %7, i64 %indvars.iv.next
  %26 = load i16, i16* %add.ptr.i.1, align 2, !tbaa !13
  %conv36.1 = sitofp i16 %26 to float
  %div37.1 = fmul fast float %conv36.1, 0x3F00002000000000
  %arrayidx40.1 = getelementptr inbounds float, float* %14, i64 %indvars.iv.next
  store float %div37.1, float* %arrayidx40.1, align 4, !tbaa !15
  %indvars.iv.next.1 = add nuw nsw i64 %indvars.iv, 2
  %niter.nsub.1 = add i64 %niter, -2
  %niter.ncmp.1 = icmp eq i64 %niter.nsub.1, 0
  br i1 %niter.ncmp.1, label %if.then.i.i.i74.loopexit.unr-lcssa, label %for.body

cleanup:                                          ; preds = %for.cond.preheader, %invoke.cont18
  %retval.0 = phi i1 [ false, %invoke.cont18 ], [ true, %for.cond.preheader ]
  %tobool.i.i.i73 = icmp eq i16* %7, null
  br i1 %tobool.i.i.i73, label %_ZNSt6vectorIsSaIsEED2Ev.exit75, label %if.then.i.i.i74

if.then.i.i.i74.loopexit.unr-lcssa:               ; preds = %for.body, %for.body.lr.ph
  %indvars.iv.unr = phi i64 [ 0, %for.body.lr.ph ], [ %indvars.iv.next.1, %for.body ]
  %lcmp.mod = icmp eq i64 %xtraiter, 0
  br i1 %lcmp.mod, label %if.then.i.i.i74, label %for.body.epil

for.body.epil:                                    ; preds = %if.then.i.i.i74.loopexit.unr-lcssa
  %add.ptr.i91.epil = getelementptr inbounds i16, i16* %9, i64 %indvars.iv.unr
  %27 = load i16, i16* %add.ptr.i91.epil, align 2, !tbaa !13
  %conv29.epil = sitofp i16 %27 to float
  %div.epil = fmul fast float %conv29.epil, 0x3F00002000000000
  %arrayidx31.epil = getelementptr inbounds float, float* %13, i64 %indvars.iv.unr
  store float %div.epil, float* %arrayidx31.epil, align 4, !tbaa !15
  %add.ptr.i.epil = getelementptr inbounds i16, i16* %7, i64 %indvars.iv.unr
  %28 = load i16, i16* %add.ptr.i.epil, align 2, !tbaa !13
  %conv36.epil = sitofp i16 %28 to float
  %div37.epil = fmul fast float %conv36.epil, 0x3F00002000000000
  %arrayidx40.epil = getelementptr inbounds float, float* %14, i64 %indvars.iv.unr
  store float %div37.epil, float* %arrayidx40.epil, align 4, !tbaa !15
  br label %if.then.i.i.i74

if.then.i.i.i74:                                  ; preds = %for.body.epil, %if.then.i.i.i74.loopexit.unr-lcssa, %cleanup
  %retval.0119 = phi i1 [ %retval.0, %cleanup ], [ true, %if.then.i.i.i74.loopexit.unr-lcssa ], [ true, %for.body.epil ]
  call void @_ZdlPv(i8* nonnull %8) #7
  br label %_ZNSt6vectorIsSaIsEED2Ev.exit75

_ZNSt6vectorIsSaIsEED2Ev.exit75:                  ; preds = %cleanup, %if.then.i.i.i74
  %retval.0120 = phi i1 [ %retval.0, %cleanup ], [ %retval.0119, %if.then.i.i.i74 ]
  %tobool.i.i.i73.1 = icmp eq i16* %9, null
  br i1 %tobool.i.i.i73.1, label %_ZNSt6vectorIsSaIsEED2Ev.exit75.1, label %if.then.i.i.i74.1

if.then.i.i.i:                                    ; preds = %lpad17
  call void @_ZdlPv(i8* nonnull %8) #7
  br label %_ZNSt6vectorIsSaIsEED2Ev.exit

_ZNSt6vectorIsSaIsEED2Ev.exit:                    ; preds = %lpad17, %if.then.i.i.i
  %tobool.i.i.i.1 = icmp eq i16* %9, null
  br i1 %tobool.i.i.i.1, label %ehcleanup56, label %if.then.i.i.i.1

ehcleanup56:                                      ; preds = %_ZNSt6vectorIsSaIsEED2Ev.exit, %if.then.i.i.i.1, %_ZNSt6vectorIsSaIsEED2Ev.exit95, %ehcleanup.thread
  %.sink = phi { i8*, i32 } [ %19, %_ZNSt6vectorIsSaIsEED2Ev.exit95 ], [ %17, %ehcleanup.thread ], [ %21, %if.then.i.i.i.1 ], [ %21, %_ZNSt6vectorIsSaIsEED2Ev.exit ]
  %exn.slot.1 = phi i8* [ %20, %_ZNSt6vectorIsSaIsEED2Ev.exit95 ], [ %18, %ehcleanup.thread ], [ %22, %if.then.i.i.i.1 ], [ %22, %_ZNSt6vectorIsSaIsEED2Ev.exit ]
  %29 = extractvalue { i8*, i32 } %.sink, 1
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %1) #7
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %0) #7
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn.slot.1, 0
  %lpad.val61 = insertvalue { i8*, i32 } %lpad.val, i32 %29, 1
  resume { i8*, i32 } %lpad.val61

if.then.i.i.i74.1:                                ; preds = %_ZNSt6vectorIsSaIsEED2Ev.exit75
  call void @_ZdlPv(i8* nonnull %10) #7
  br label %_ZNSt6vectorIsSaIsEED2Ev.exit75.1

_ZNSt6vectorIsSaIsEED2Ev.exit75.1:                ; preds = %if.then.i.i.i74.1, %_ZNSt6vectorIsSaIsEED2Ev.exit75
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %1) #7
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %0) #7
  ret i1 %retval.0120

if.then.i.i.i.1:                                  ; preds = %_ZNSt6vectorIsSaIsEED2Ev.exit
  call void @_ZdlPv(i8* nonnull %10) #7
  br label %ehcleanup56
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #2

declare dso_local float @_Z16RadiansToDegreesf(float) local_unnamed_addr #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #2

declare dso_local i32 @mit_hrtf_get(i32*, i32*, i32, i16*, i16*) local_unnamed_addr #1

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr dso_local void @_ZN8MIT_HRTFD0Ev(%class.MIT_HRTF* %this) unnamed_addr #3 comdat align 2 {
entry:
  %0 = bitcast %class.MIT_HRTF* %this to i8*
  tail call void @_ZdlPv(i8* %0) #8
  ret void
}

; Function Attrs: nounwind uwtable
define linkonce_odr dso_local void @_ZN4HRTFD2Ev(%class.HRTF* %this) unnamed_addr #4 comdat align 2 {
entry:
  ret void
}

; Function Attrs: nobuiltin nounwind
declare dso_local void @_ZdlPv(i8*) local_unnamed_addr #5

; Function Attrs: nobuiltin nofree
declare dso_local noalias nonnull i8* @_Znwm(i64) local_unnamed_addr #6

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }
attributes #3 = { inlinehint nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #4 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #5 = { nobuiltin nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #6 = { nobuiltin nofree "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { builtin nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (https://gitlab.engr.illinois.edu/llvm/hpvm-release.git f83c355d11be7d011508763fedc67e76d592a636)"}
!2 = !{!3, !4, i64 8}
!3 = !{!"_ZTS4HRTF", !4, i64 8, !4, i64 12}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C++ TBAA"}
!7 = !{!3, !4, i64 12}
!8 = !{!9, !9, i64 0}
!9 = !{!"vtable pointer", !6, i64 0}
!10 = !{!4, !4, i64 0}
!11 = !{!12, !12, i64 0}
!12 = !{!"any pointer", !5, i64 0}
!13 = !{!14, !14, i64 0}
!14 = !{!"short", !5, i64 0}
!15 = !{!16, !16, i64 0}
!16 = !{!"float", !5, i64 0}
